#include "isl/ctx.h"
#include "isl/flow.h"
#include "isl/map.h"
#include "isl/options.h"
#include "isl/space.h"
#include "isl/union_map.h"
#include <assert.h>
#include <iostream>

struct must_may {
  isl_map *must;
  isl_map *may;
};

static int common_space(void *first, void *second) {
  int depth = *(int *)first;
  return 2 * depth;
}

static isl_stat collect_must_may(__isl_take isl_map *dep, int must,
                                 void *dep_user, void *user) {
  struct must_may *mm = (struct must_may *)user;

  if (must)
    mm->must = isl_map_union(mm->must, dep);
  else
    mm->may = isl_map_union(mm->may, dep);

  return isl_stat_ok;
}

static int map_is_equal(__isl_keep isl_map *map, const char *str) {
  isl_map *map2;
  int equal;

  if (!map)
    return -1;

  map2 = isl_map_read_from_str(isl_map_get_ctx(map), str);
  equal = isl_map_is_equal(map, map2);
  isl_map_free(map2);

  return equal;
}

static int test_dep(struct isl_ctx *ctx) {
  const char *str;
  isl_space *space;
  isl_map *map;
  isl_access_info *ai;
  isl_flow *flow;
  int depth;
  struct must_may mm;

  depth = 3;

  str = "{ [2,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 2);

  str = "{ [0,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 1, &depth);

  str = "{ [1,i,0] -> [5] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 1, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 3, 3);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0] -> [2,i,0] : (0 <= i <= 4) or (6 <= i <= 10); "
        "  [1,10,0] -> [2,5,0] }";
  assert(map_is_equal(mm.must, str));
  str = "{ [i,j,k] -> [l,m,n] : 1 = 0 }";
  assert(map_is_equal(mm.may, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  str = "{ [2,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 2);

  str = "{ [0,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 1, &depth);

  str = "{ [1,i,0] -> [5] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 3, 3);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0] -> [2,i,0] : (0 <= i <= 4) or (6 <= i <= 10) }";
  assert(map_is_equal(mm.must, str));
  str = "{ [0,5,0] -> [2,5,0]; [1,i,0] -> [2,5,0] : 0 <= i <= 10 }";
  assert(map_is_equal(mm.may, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  str = "{ [2,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 2);

  str = "{ [0,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  str = "{ [1,i,0] -> [5] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 3, 3);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0] -> [2,i,0] : 0 <= i <= 10; "
        "  [1,i,0] -> [2,5,0] : 0 <= i <= 10 }";
  assert(map_is_equal(mm.may, str));
  str = "{ [i,j,k] -> [l,m,n] : 1 = 0 }";
  assert(map_is_equal(mm.must, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  str = "{ [0,i,2] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 2);

  str = "{ [0,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  str = "{ [0,i,1] -> [5] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 3, 3);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0] -> [0,i,2] : 0 <= i <= 10; "
        "  [0,i,1] -> [0,5,2] : 0 <= i <= 5 }";
  assert(map_is_equal(mm.may, str));
  str = "{ [i,j,k] -> [l,m,n] : 1 = 0 }";
  assert(map_is_equal(mm.must, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  str = "{ [0,i,1] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 2);

  str = "{ [0,i,0] -> [i] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  str = "{ [0,i,2] -> [5] : 0 <= i <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 0, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 3, 3);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0] -> [0,i,1] : 0 <= i <= 10; "
        "  [0,i,2] -> [0,5,1] : 0 <= i <= 4 }";
  assert(map_is_equal(mm.may, str));
  str = "{ [i,j,k] -> [l,m,n] : 1 = 0 }";
  assert(map_is_equal(mm.must, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  depth = 5;

  str = "{ [1,i,0,0,0] -> [i,j] : 0 <= i <= 10 and 0 <= j <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_alloc(map, &depth, &common_space, 1);

  str = "{ [0,i,0,j,0] -> [i,j] : 0 <= i <= 10 and 0 <= j <= 10 }";
  map = isl_map_read_from_str(ctx, str);
  ai = isl_access_info_add_source(ai, map, 1, &depth);

  flow = isl_access_info_compute_flow(ai);
  space = isl_space_alloc(ctx, 0, 5, 5);
  mm.must = isl_map_empty(isl_space_copy(space));
  mm.may = isl_map_empty(space);

  isl_flow_foreach(flow, collect_must_may, &mm);

  str = "{ [0,i,0,j,0] -> [1,i,0,0,0] : 0 <= i,j <= 10 }";
  assert(map_is_equal(mm.must, str));
  str = "{ [0,0,0,0,0] -> [0,0,0,0,0] : 1 = 0 }";
  assert(map_is_equal(mm.may, str));

  isl_map_free(mm.must);
  isl_map_free(mm.may);
  isl_flow_free(flow);

  return 0;
}

int main() {
  std::cout << "Hello, polyhedral model!" << std::endl;

  struct isl_ctx *ctx;
  struct isl_options *options;

  options = isl_options_new_with_defaults();
  assert(options);
  ctx = isl_ctx_alloc_with_options(&isl_options_args, options);
  test_dep(ctx);
  return 0;
}
