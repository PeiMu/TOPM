//
// Created by SENSETIME\mupei on 2021/8/24.
//

#include "manual_case.h"

static inline void Values(isl_ctx *ctx) {
  auto si_val_num = isl_val_int_from_si(ctx, -2);
  isl_dump(si_val_num);
  auto ui_val_den = isl_val_int_from_ui(ctx, 5);
  isl_dump(ui_val_den);

  auto fraction_val = isl_val_div(si_val_num, ui_val_den);
  isl_dump(fraction_val);

  auto si_num = isl_val_get_num_si(fraction_val);
  printf("si_num = %ld\n", si_num);
  auto si_den = isl_val_get_den_si(fraction_val);
  printf("si_den = %ld\n", si_den);

  int arr[2] = {1, 2};
  auto val = isl_val_int_from_chunks(ctx, 2, sizeof(int), arr);

  isl_dump(val);
  auto size = isl_val_n_abs_num_chunks(val, sizeof(int));
  printf("size = %d\n", size);
  int test[10] = {0};
  auto stat = isl_val_get_abs_num_chunks(val, sizeof(int), test);
  isl_assert(ctx, stat == isl_stat_ok, isl_ctx_set_error(ctx, isl_error_abort));
  size_t i = 0;
  while (test[i] != 0) {
    printf("test[%zu] = %d, ", i, test[i]);
    i++;
  }
  printf("\n");
}

static inline void UnnamedParamSpaces(isl_ctx *ctx) {
  auto unit = isl_space_unit(ctx);
  isl_dump(unit);
  isl_assert(ctx, isl_bool_true == isl_space_is_params(unit) &&
                      isl_bool_true == isl_space_is_set(unit),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == isl_space_dim(unit, isl_dim_param),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == isl_space_dim(unit, isl_dim_set),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == isl_space_dim(unit, isl_dim_all),
             isl_ctx_set_error(ctx, isl_error_abort));

  // 不建议使用这种方法，因为它们会使用未命名的parameter。
  auto space_pio = isl_space_alloc(ctx, 2, 4, 4);
  isl_dump(space_pio);
  isl_assert(ctx, isl_bool_true == isl_space_is_map(space_pio),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 2 == isl_space_dim(space_pio, isl_dim_param),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 4 == isl_space_dim(space_pio, isl_dim_in),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 4 == isl_space_dim(space_pio, isl_dim_out),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 10 == isl_space_dim(space_pio, isl_dim_all),
             isl_ctx_set_error(ctx, isl_error_abort));

  auto space_p = isl_space_params_alloc(ctx, 3);
  isl_dump(space_p);
  isl_assert(ctx, isl_bool_true == isl_space_is_params(space_p) &&
                      isl_bool_true == isl_space_is_set(space_p),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 3 == isl_space_dim(space_p, isl_dim_param),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == isl_space_dim(space_p, isl_dim_set),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 3 == isl_space_dim(space_p, isl_dim_all),
             isl_ctx_set_error(ctx, isl_error_abort));

  auto space_set = isl_space_set_alloc(ctx, 2, 3);
  isl_dump(space_set);
  isl_assert(ctx, isl_bool_true == isl_space_is_set(space_set),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 2 == isl_space_dim(space_set, isl_dim_param),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 3 == isl_space_dim(space_set, isl_dim_set),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 5 == isl_space_dim(space_set, isl_dim_all),
             isl_ctx_set_error(ctx, isl_error_abort));

  isl_assert(ctx,
             isl_bool_true == isl_space_has_equal_params(space_pio, space_set),
             isl_ctx_set_error(ctx, isl_error_abort));
}

static inline void SpacesDim(isl_ctx *ctx) {
  auto space = isl_space_unit(ctx);

  // add param id
  auto id_i = isl_id_alloc(ctx, "i", nullptr);
  space = isl_space_add_param_id(space, isl_id_copy(id_i));
  isl_dump(space);

  // drop param id
  space = isl_space_drop_all_params(space);
  isl_dump(space);

  // add/set/get/delete dim id
  auto id_j = isl_id_alloc(ctx, "j", nullptr);
  // set_dim_id只能覆盖，不能新增。新增需要通过add_dims
//  space = isl_space_set_dim_id(space, isl_dim_set, 0, isl_id_copy(id_i));
    space = isl_space_add_dims(space, isl_dim_param, 1);
  space = isl_space_set_dim_id(space, isl_dim_param, 0, isl_id_copy(id_j));
  isl_assert(ctx, isl_bool_true == isl_space_has_dim_id(space, isl_dim_param, 0),

              isl_ctx_set_error(ctx, isl_error_abort));
  id_j = isl_space_get_dim_id(space, isl_dim_param, 0);
  isl_assert(ctx, 0 == strcmp("j", isl_id_get_name(id_j)),
             isl_ctx_set_error(ctx, isl_error_abort));

  space = isl_space_add_dims(space, isl_dim_set, 1);
  space = isl_space_set_dim_id(space, isl_dim_set, 0, isl_id_copy(id_i));
  isl_dump(space);
  space = isl_space_drop_dims(space, isl_dim_set, 0, 1);
  isl_dump(space);

  space = isl_space_add_dims(space, isl_dim_in, 2);
  space = isl_space_set_dim_id(space, isl_dim_in, 0, isl_id_copy(id_i));
  space = isl_space_set_dim_id(space, isl_dim_in, 1, isl_id_copy(id_j));
  space = isl_space_add_dims(space, isl_dim_out, 1);
  space = isl_space_set_dim_id(space, isl_dim_out, 0, isl_id_copy(id_i));
  isl_dump(space);

  // add/set/get/delete dim name
  space = isl_space_set_dim_name(space, isl_dim_param, 0, "param_j");
  isl_dump(space);
  isl_assert(ctx, isl_bool_true == isl_space_has_dim_name(space, isl_dim_in, 1),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == strcmp("i", isl_space_get_dim_name(space, isl_dim_out, 0)),

               isl_ctx_set_error(ctx, isl_error_abort));

  // find dim
  isl_assert(ctx, 1 == isl_space_find_dim_by_id(space, isl_dim_in, id_j),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, 0 == isl_space_find_dim_by_name(space, isl_dim_out, "i"),
             isl_ctx_set_error(ctx, isl_error_abort));

  // set tuple domain/range id or name
  auto domain_id = isl_id_alloc(ctx, "domain_id", nullptr);
  auto range_id = isl_id_alloc(ctx, "range_id", nullptr);
  space = isl_space_set_domain_tuple_id(space, domain_id);
  isl_dump(space);
  space = isl_space_set_range_tuple_id(space, range_id);
  isl_dump(space);

  space = isl_space_reset_tuple_id(space, isl_dim_in);
  space = isl_space_reset_tuple_id(space, isl_dim_out);
  isl_dump(space);

  isl_assert(ctx, isl_bool_false == isl_space_has_domain_tuple_id(space),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, isl_bool_false == isl_space_has_tuple_id(space, isl_dim_out),
             isl_ctx_set_error(ctx, isl_error_abort));

  space = isl_space_set_tuple_name(space, isl_dim_in, "in");
  space = isl_space_set_tuple_name(space, isl_dim_out, "out");
  isl_dump(space);

  isl_dump(isl_space_get_tuple_id(space, isl_dim_in));
  isl_dump(isl_space_get_range_tuple_id(space));

  // local space
  auto local_space = isl_local_space_from_space(isl_space_copy(space));
  isl_dump(local_space);
  isl_assert(ctx, isl_bool_false == isl_local_space_is_set(local_space),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, isl_bool_true == isl_space_is_equal(space, isl_local_space_get_space(local_space)),
             isl_ctx_set_error(ctx, isl_error_abort));
}

isl_stat PrintBasicSet(__isl_take isl_basic_set *bset, void *user) {
  isl_dump(bset);
  return isl_stat_ok;
}

static inline void Sets(isl_ctx* ctx) {
  auto set_space = isl_space_unit(ctx);
  auto id_i = isl_id_alloc(ctx, "id_i", nullptr);
  auto id_j = isl_id_alloc(ctx, "id_j", nullptr);
  set_space = isl_space_add_param_id(set_space, id_i);
  set_space = isl_space_add_param_id(set_space, id_j);
  set_space = isl_space_add_dims(set_space, isl_dim_set, 3);
  auto idx_0 = isl_id_alloc(ctx, "idx_0", nullptr);
  auto idx_1 = isl_id_alloc(ctx, "idx_1", nullptr);
  auto idx_2 = isl_id_alloc(ctx, "idx_2", nullptr);
  set_space = isl_space_set_dim_id(set_space, isl_dim_set, 0, idx_0);
  set_space = isl_space_set_dim_id(set_space, isl_dim_set, 1, idx_1);
  set_space = isl_space_set_dim_id(set_space, isl_dim_set, 2, idx_2);
  set_space = isl_space_set_tuple_name(set_space, isl_dim_set, "S_0");
  isl_dump(set_space);

  auto empty_basic_set = isl_basic_set_empty(isl_space_copy(set_space));
  isl_dump(empty_basic_set);
  isl_dump(isl_set_from_basic_set(empty_basic_set));

  auto empty_set = isl_set_empty(isl_space_copy(set_space));
  isl_dump(empty_set);
  isl_assert(ctx, 0 == isl_set_n_basic_set(empty_set),
             isl_ctx_set_error(ctx, isl_error_abort));

  auto empty_union_set = isl_union_set_empty(isl_space_copy(set_space));
  isl_dump(isl_union_set_empty_ctx(ctx));
  isl_dump(empty_union_set);
  isl_assert(ctx, 0 == isl_union_set_n_set(empty_union_set),
             isl_ctx_set_error(ctx, isl_error_abort));

  auto universe_basic_set = isl_basic_set_universe(isl_space_copy(set_space));
  isl_dump(universe_basic_set);
  isl_dump(isl_set_universe(isl_space_copy(set_space)));

  auto universe_set = isl_set_universe(isl_space_copy(set_space));
  isl_dump(universe_set);
  isl_assert(ctx, 1 == isl_set_n_basic_set(universe_set),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_set_foreach_basic_set(universe_set, PrintBasicSet, nullptr);

  auto bset_list = isl_set_get_basic_set_list(universe_set);
  isl_dump(bset_list);

  auto universe_union_set = isl_union_set_universe(isl_union_set_empty(isl_space_copy(set_space)));
  isl_dump(universe_union_set);
  isl_assert(ctx, 0 == isl_union_set_n_set(universe_union_set),
             isl_ctx_set_error(ctx, isl_error_abort));

  isl_dump(isl_basic_set_nat_universe(isl_space_copy(set_space)));
  isl_dump(isl_set_nat_universe(isl_space_copy(set_space)));

  // lexico
  isl_dump(isl_map_lex_lt(isl_space_copy(set_space)));
}

static inline void Maps(isl_ctx* ctx) {
  auto map_space = isl_space_unit(ctx);
  auto id_k = isl_id_alloc(ctx, "id_k", nullptr);
  map_space = isl_space_add_param_id(map_space, id_k);
  map_space = isl_space_add_dims(map_space, isl_dim_in, 1);
  map_space = isl_space_add_dims(map_space, isl_dim_out, 1);
  auto idx_in = isl_id_alloc(ctx, "idx_in", nullptr);
  auto idx_out = isl_id_alloc(ctx, "idx_out", nullptr);
  map_space = isl_space_set_dim_id(map_space, isl_dim_in, 0, idx_in);
  map_space = isl_space_set_dim_id(map_space, isl_dim_out, 0, idx_out);
  map_space = isl_space_set_tuple_name(map_space, isl_dim_in, "S_0");
  map_space = isl_space_set_tuple_name(map_space, isl_dim_out, "Var_0");
  isl_dump(map_space);

  isl_dump(isl_basic_map_empty(isl_space_copy(map_space)));
  isl_dump(isl_map_from_basic_map(isl_basic_map_empty(isl_space_copy(map_space))));
  isl_dump(isl_map_empty(isl_space_copy(map_space)));
  isl_dump(isl_union_map_empty_ctx(ctx));
  isl_dump(isl_union_map_empty(isl_space_copy(map_space)));

  isl_dump(isl_basic_map_universe(isl_space_copy(map_space)));
  isl_dump(isl_map_universe(isl_space_copy(map_space)));
  isl_dump(isl_union_map_universe(isl_union_map_empty(isl_space_copy(map_space))));

  isl_dump(isl_basic_map_nat_universe(isl_space_copy(map_space)));
  isl_dump(isl_map_nat_universe(isl_space_copy(map_space)));

  // identity
  isl_dump(isl_basic_map_identity(isl_space_copy(map_space)));
  isl_dump(isl_map_identity(isl_space_copy(map_space)));

  // lexico
  isl_dump(isl_map_lex_lt_first(isl_space_copy(map_space), 0));
}

// low level approach
// create a set containing the even integers between 10 and 42
void SetConstraint1(isl_ctx* ctx) {
  auto space = isl_space_set_alloc(ctx, 0, 2);
  isl_dump(space);
  auto bset = isl_basic_set_universe(isl_space_copy(space));
  isl_dump(bset);
  auto ls = isl_local_space_from_space(space);
  isl_dump(ls);

  // even number constraint, 2i1 = i0
  auto c = isl_constraint_alloc_equality(isl_local_space_copy(ls));
  isl_dump(c);
  c = isl_constraint_set_coefficient_si(c, isl_dim_set, 0, -1);
  isl_dump(c);
  c = isl_constraint_set_coefficient_si(c, isl_dim_set, 1, 2);
  isl_dump(c);
  bset = isl_basic_set_add_constraint(bset, c);
  isl_dump(bset);

  // i0 >= 10
  c = isl_constraint_alloc_inequality(isl_local_space_copy(ls));
  isl_dump(c);
  c = isl_constraint_set_constant_si(c, -10);
  isl_dump(c);
  c = isl_constraint_set_coefficient_si(c, isl_dim_set, 0, 1);
  isl_dump(c);
  bset = isl_basic_set_add_constraint(bset, c);
  isl_dump(bset);

  // i0 <=42
  c = isl_constraint_alloc_inequality(isl_local_space_copy(ls));
  c = isl_constraint_set_constant_si(c, 42);
  isl_dump(c);
  c = isl_constraint_set_coefficient_si(c, isl_dim_set, 0, -1);
  isl_dump(c);
  bset = isl_basic_set_add_constraint(bset, c);
  isl_dump(bset);

  bset = isl_basic_set_project_out(bset, isl_dim_set, 1, 1);
  isl_dump(bset);
}

isl_stat PrintConstraint(__isl_take isl_constraint* cst, void *user) {
  isl_dump(cst);
  isl_dump(isl_constraint_get_constant_val(cst));
  isl_dump(isl_constraint_get_coefficient_val(cst, isl_dim_set, 0));
  isl_dump(isl_constraint_get_div(cst, 0));
  return isl_stat_ok;
}

// affine expressions
// create a set containing the even integers between 10 and 42
void SetConstraint2(isl_ctx* ctx) {
  auto space = isl_space_unit(ctx);
  space = isl_space_add_dims(space, isl_dim_set, 1);
  isl_dump(space);

  auto ma = isl_multi_aff_identity_on_domain_space(isl_space_copy(space));
  isl_dump(ma);

  auto var = isl_multi_aff_get_at(ma, 0);
  isl_dump(var);

  // i0 >= 10
  auto v = isl_val_int_from_si(ctx, 10);
  auto cst = isl_aff_val_on_domain_space(isl_space_copy(space), v);
  auto bset = isl_aff_ge_basic_set(isl_aff_copy(var), cst);
  isl_dump(bset);

  // i0 >= 10 and i0 <= 42
  v = isl_val_int_from_si(ctx, 42);
  cst = isl_aff_val_on_domain_space(space, v);
  bset = isl_basic_set_intersect(bset, isl_aff_le_basic_set(var, cst));
  isl_dump(bset);

  // even number and i0 >= 10 and i0 <= 42
  auto two = isl_val_int_from_si(ctx, 2);
  ma = isl_multi_aff_scale_val(ma, isl_val_copy(two));
  bset = isl_basic_set_preimage_multi_aff(bset, isl_multi_aff_copy(ma));
  ma = isl_multi_aff_scale_down_val(ma, isl_val_copy(two));
  ma = isl_multi_aff_scale_down_val(ma, two);
  bset = isl_basic_set_preimage_multi_aff(bset, ma);
  isl_dump(bset);

  isl_dump(isl_basic_set_remove_divs(isl_basic_set_copy(bset)));
  isl_dump(isl_basic_set_remove_divs_involving_dims(isl_basic_set_copy(bset), isl_dim_set, 0, 1));

  isl_assert(ctx, 3 == isl_basic_set_n_constraint(bset),
             isl_ctx_set_error(ctx, isl_error_abort));

  isl_basic_set_foreach_constraint(bset, PrintConstraint, nullptr);

  auto constraint_list = isl_basic_set_get_constraint_list(bset);
  isl_assert(ctx, isl_bool_true ==
                  isl_constraint_is_equality(
                          isl_constraint_list_get_constraint(constraint_list, 0)),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, isl_bool_false ==
                  isl_constraint_is_equality(
                          isl_constraint_list_get_constraint(constraint_list, 1)),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, isl_bool_true ==
                  isl_constraint_is_lower_bound(
                          isl_constraint_list_get_constraint(constraint_list, 1),
                          isl_dim_set, 0),
             isl_ctx_set_error(ctx, isl_error_abort));
  isl_assert(ctx, isl_bool_true ==
                  isl_constraint_is_upper_bound(
                          isl_constraint_list_get_constraint(constraint_list, 2),
                          isl_dim_set, 0),
             isl_ctx_set_error(ctx, isl_error_abort));

  isl_dump(isl_basic_set_equalities_matrix(
          bset, isl_dim_cst, isl_dim_param, isl_dim_set, isl_dim_div));
  isl_dump(isl_basic_set_inequalities_matrix(
          bset, isl_dim_div, isl_dim_set, isl_dim_param, isl_dim_cst));
}

void ManualCase() {
  struct isl_options *options = isl_options_new_with_defaults();
  assert(options);
  struct isl_ctx *ctx = isl_ctx_alloc_with_options(&isl_options_args, options);

//  Values(ctx);

//  UnnamedParamSpaces(ctx);
//  SpacesDim(ctx);

//  Sets(ctx);
//  Maps(ctx);

//  SetConstraint1(ctx);
  SetConstraint2(ctx);
}
