#include "isl_dump.h"
#include "isl/ast.h"
#include "isl/ast_type.h"
#include "isl/id.h"
#include "isl/options.h"
#include "isl/space.h"
#include "isl/union_map.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <isl/map.h>
#include <tuple>
#include <unordered_map>
#include <vector>

static std::tuple<isl_set *, isl_id_list *, isl_aff *>
ComputeLoopConstrain(isl_set *set, isl_id_list *id_list, std::string iter_name,
                     int min, int max) {
  auto loop_var_id =
      isl_id_read_from_str(isl_set_get_ctx(set), iter_name.c_str());
  auto loop_var_space =
      isl_space_add_param_id(isl_set_get_space(set), isl_id_copy(loop_var_id));
  auto loop_var = isl_aff_param_on_domain_space_id(loop_var_space, loop_var_id);

  // Add lower and upper loop bound constraints.
  std::vector<isl_aff *> constraint_bounds = {isl_aff_val_on_domain(
      isl_local_space_from_space(loop_var_space),
      isl_val_int_from_si(isl_space_get_ctx(loop_var_space), min))};
  for (const auto &item : constraint_bounds) {
    auto tmp = isl_aff_ge_set(isl_aff_copy(loop_var), isl_aff_copy(item));
    set = isl_set_intersect(set, tmp);
  }

  constraint_bounds.clear();
  constraint_bounds = {isl_aff_val_on_domain(
      isl_local_space_from_space(loop_var_space),
      isl_val_int_from_si(isl_space_get_ctx(loop_var_space), max))};
  for (const auto &item : constraint_bounds) {
    auto tmp = isl_aff_le_set(isl_aff_copy(loop_var), isl_aff_copy(item));
    set = isl_set_intersect(set, tmp);
  }
  isl_dump(set);

  auto outer_add = isl_id_list_add(isl_id_list_copy(id_list), loop_var_id);

  isl_dump(outer_add);
  std::tuple<isl_set *, isl_id_list *, isl_aff *> ret{set, outer_add, loop_var};
  return ret;
}

std::unordered_map<std::string, isl_id *> accesses;

static std::tuple<isl_union_map *, isl_union_map *, isl_union_map *>
MemoryAccess(isl_space *domain_space, isl_multi_id *domain_tuple) {
  isl_union_map *reads =
      isl_union_map_empty(isl_multi_id_get_space(domain_tuple));
  isl_union_map *writes =
      isl_union_map_empty(isl_multi_id_get_space(domain_tuple));
  isl_union_map *to_inner =
      isl_union_map_empty(isl_multi_id_get_space(domain_tuple));

  // find from inner to outer
  // create a tensor coordinate to store the accessed relation
  isl_id_list *args = isl_id_list_alloc(isl_space_get_ctx(domain_space), 0);
  const int dim = 2;
  for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
    auto name = std::string("iter") + std::to_string(dim_idx);
    args = isl_id_list_add(
        isl_id_list_copy(args),
        isl_id_read_from_str(isl_space_get_ctx(domain_space), name.c_str()));
  }
  auto id = isl_id_read_from_str(isl_space_get_ctx(domain_space), "Var2");

  auto tmp_space = isl_space_add_named_tuple_id_ui(domain_space, id, dim);

  auto coordinate = isl_multi_id_from_id_list(tmp_space, args);
  auto tensor_space = isl_multi_id_get_space(coordinate);

  // create a fully access set
  auto tensor_access = isl_set_universe(tensor_space);

  // add access relation constraint for each parameter of one dimension
  auto identity = isl_multi_aff_identity(
      isl_space_map_from_set(isl_space_copy(tensor_space)));

  // make aff bounds of each dimension.
  auto var0_tmp_id =
      isl_id_read_from_str(isl_space_get_ctx(domain_space), "iter0");

  std::vector<isl_aff *> var0_list = {
      isl_aff_param_on_domain_space_id(domain_space, var0_tmp_id)};
  auto domain_aff_bounds = var0_list.empty() ? nullptr : var0_list[0];
  if (domain_aff_bounds != nullptr) {
    domain_aff_bounds = isl_aff_unbind_params_insert_domain(
        isl_aff_copy(domain_aff_bounds), coordinate);
    auto identity_aff = isl_multi_aff_get_aff(identity, 0);
    auto tmp_set =
        isl_aff_eq_set(isl_aff_copy(domain_aff_bounds), identity_aff);
    tensor_access = isl_set_intersect(isl_set_copy(tensor_access), tmp_set);
  }

  auto var1_tmp_id =
      isl_id_read_from_str(isl_space_get_ctx(domain_space), "iter1");
  std::vector<isl_aff *> var1_list = {
      isl_aff_param_on_domain_space_id(domain_space, var1_tmp_id)};
  domain_aff_bounds = var1_list.empty() ? nullptr : var1_list[0];
  if (domain_aff_bounds != nullptr) {
    domain_aff_bounds = isl_aff_unbind_params_insert_domain(
        isl_aff_copy(domain_aff_bounds), coordinate);
    auto tmp_set = isl_aff_eq_set(isl_aff_copy(domain_aff_bounds),
                                  isl_multi_aff_get_aff(identity, 1));
    tensor_access = isl_set_intersect(isl_set_copy(tensor_access), tmp_set);
  }

  // add suffix for accesses
  auto tensor_map = isl_set_unbind_params_insert_domain(
      isl_set_copy(tensor_access), domain_tuple);
  // Based on different condition, add suffix to the domain space
  std::string suffix = "__poly_ref_" + std::to_string(accesses.size());
  isl_id *suffix_id =
      isl_id_read_from_str(isl_space_get_ctx(domain_space), suffix.c_str());
  auto suffix_domain_space =
      isl_space_domain(isl_space_copy(isl_map_get_space(tensor_map)));
  auto tag_space = isl_space_add_named_tuple_id_ui(
      isl_space_copy(isl_space_params(isl_space_copy(suffix_domain_space))),
      suffix_id, 0);
  domain_space = isl_space_unwrap(isl_space_copy(
      isl_space_product(isl_space_copy(domain_space), tag_space)));
  tensor_map = isl_map_preimage_domain_multi_aff(
      isl_map_copy(tensor_map), isl_multi_aff_domain_map(domain_space));
  auto identity_map =
      isl_map_from_pw_multi_aff(isl_pw_multi_aff_from_multi_aff(identity));
  writes = isl_union_map_union(isl_union_map_copy(writes),
                               isl_union_map_from_map(tensor_map));
  to_inner = isl_union_map_add_map(isl_union_map_copy(to_inner), identity_map);
  return std::make_tuple(reads, writes, to_inner);
}

static int TestStore(isl_set *set, isl_id_list *id_list) {
  auto id = isl_id_read_from_str(isl_set_get_ctx(set), "S_0");
  auto tuple_space = isl_space_params_alloc(isl_set_get_ctx(set), 0);
  tuple_space = isl_space_add_named_tuple_id_ui(isl_space_copy(tuple_space), id,
                                                isl_id_list_size(id_list));
  auto domain_space = isl_set_get_space(set);
  auto domain_tuple = isl_multi_id_from_id_list(tuple_space, id_list);
  auto domain = isl_set_unbind_params(set, isl_multi_id_copy(domain_tuple));
  auto schedule = isl_schedule_from_domain(isl_union_set_from_set(domain));
  isl_dump(schedule);
  isl_set_free(set);

  isl_union_map *new_reads, *new_writes, *new_to_inner;
  std::tie(new_reads, new_writes, new_to_inner) =
      MemoryAccess(domain_space, domain_tuple);

  return 0;
}

static int TestFor(isl_set *set, isl_id_list *id_list) {
  auto outer_constrian = ComputeLoopConstrain(set, id_list, "iter0", 0, 63);

  auto inner_constrain =
      ComputeLoopConstrain(std::get<0>(outer_constrian),
                           std::get<1>(outer_constrian), "iter1", 0, 7);

  TestStore(std::get<0>(inner_constrain), std::get<1>(inner_constrain));
  // todo: rewrite after visit store
  isl_schedule *inner_schedule = nullptr;
  auto inner_domain = isl_schedule_get_domain(inner_schedule);
  auto inner_union_pw_aff =
      isl_union_pw_aff_empty(isl_union_set_get_space(inner_domain));
  auto union_set_list = isl_union_set_get_set_list(inner_domain);

  auto tuple_space =
      isl_space_params_alloc(isl_set_get_ctx(std::get<0>(inner_constrain)), 0);
  isl_multi_id *multi_id =
      isl_space_multi_id(tuple_space, std::get<1>(inner_constrain));
  for (int i = 0; i < isl_set_list_size(union_set_list); i++) {
    auto union_pw_aff = isl_union_pw_aff_from_pw_aff(
        isl_pw_aff_from_aff(isl_aff_unbind_params_insert_domain(
            isl_aff_copy(std::get<2>(inner_constrain)), multi_id)));
    inner_union_pw_aff = isl_union_pw_aff_union_add(
        isl_union_pw_aff_copy(inner_union_pw_aff), union_pw_aff);
  }
  auto inner_multi_union_pw_aff_func =
      isl_multi_union_pw_aff_from_union_pw_aff(inner_union_pw_aff);
  isl_dump(inner_multi_union_pw_aff_func);

  isl_schedule *schedule = isl_schedule_insert_partial_schedule(
      isl_schedule_copy(inner_schedule), inner_multi_union_pw_aff_func);
  isl_dump(schedule);
  return 0;
}

static isl_stat test_union_set_contains(isl_ctx *ctx) {
  const char *str;
  isl_bool ok;
  isl_space *space;
  isl_id *id;
  isl_union_set *uset;

  str = "[N] -> { A[0:N]; B[*,*] }";
  uset = isl_union_set_read_from_str(ctx, str);
  space = isl_space_unit(ctx);
  id = isl_id_alloc(ctx, "A", NULL);
  auto new_space = isl_space_add_named_tuple_id_ui(space, id, 1);
  isl_dump(new_space);
  auto b = isl_space_is_equal(space, new_space);
  id = isl_id_read_from_str(isl_space_get_ctx(space), "Var2");
  auto tmp_space = isl_space_add_named_tuple_id_ui(space, id, 1);
  b = isl_space_is_equal(tmp_space, new_space);
  ok = isl_union_set_contains(uset, space);
  isl_space_free(space);
  isl_union_set_free(uset);

  if (ok < 0)
    return isl_stat_error;
  if (!ok)
    isl_die(ctx, isl_error_unknown, "unexpected result", return isl_stat_error);

  return isl_stat_ok;
}

int main() {
  struct isl_options *options = isl_options_new_with_defaults();
  assert(options);
  struct isl_ctx *ctx = isl_ctx_alloc_with_options(&isl_options_args, options);

  test_union_set_contains(ctx);

  //  assert(test_expr_compute(ctx) == 0);

  auto space = isl_space_params_alloc(ctx, 0);
  auto set = isl_set_universe(space);
  auto id_list = isl_id_list_alloc(ctx, 0);

  /**
   * for (iter0 : 0 +> 64)
   *   for (iter1 : 0 +> 8)
   *     Var2[iter0, iter1] = Var0[iter0, iter1] + Var1[iter0, iter1]
   */
  assert(TestFor(set, id_list) == 0);
  //  assert(TestStore(set, id_list) == 0);

  isl_ctx_free(ctx);

  return 0;
}
