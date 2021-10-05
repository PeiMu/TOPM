//
// Created by SENSETIME\mupei on 2021/8/10.
//

#include "isl_dump.h"

void isl_dump(isl_val *val) { isl_val_dump(val); }

void isl_dump(isl_id *id) { isl_id_dump(id); }

void isl_dump(isl_space *space) { isl_space_dump(space); }

void isl_dump(isl_local_space *local_space) {
  isl_local_space_dump(local_space);
}

void isl_dump(isl_basic_set *basic_set) { isl_basic_set_dump(basic_set); }

void isl_dump(isl_set *set) { isl_set_dump(set); }

void isl_dump(isl_union_set *union_set) { isl_union_set_dump(union_set); }

void isl_dump(isl_basic_set_list *basic_set_list) {
  isl_basic_set_list_dump(basic_set_list);
}

void isl_dump(isl_set_list *set_list) { isl_set_list_dump(set_list); }

void isl_dump(isl_basic_map *basic_map) { isl_basic_map_dump(basic_map); }

void isl_dump(isl_map *map) {
  isl_pw_multi_aff_dump(isl_map_as_pw_multi_aff(map));
}

void isl_dump(isl_union_map *union_map) { isl_union_map_dump(union_map); }

void isl_dump(isl_aff *aff) { isl_aff_dump(aff); }

void isl_dump(isl_schedule *schedule) { isl_schedule_dump(schedule); }

void isl_dump(isl_ast_expr *expr) { isl_ast_expr_dump(expr); }

void isl_dump(isl_id_list *id_list) { isl_id_list_dump(id_list); }

void isl_dump(isl_multi_union_pw_aff *multi_union_pw_aff) {
  isl_multi_union_pw_aff_dump(multi_union_pw_aff);
}

void isl_dump(isl_multi_id *multi_id) { isl_multi_id_dump(multi_id); }

void isl_dump(isl_multi_aff *multi_aff) { isl_multi_aff_dump(multi_aff); }

void isl_dump(isl_pw_multi_aff *pw_multi_aff) {
  isl_pw_multi_aff_dump(pw_multi_aff);
}

void isl_dump(isl_union_pw_multi_aff *union_pw_multi_aff) {
  isl_union_pw_multi_aff_dump(union_pw_multi_aff);
}

void isl_dump(isl_constraint *constraint) { isl_constraint_dump(constraint); }

void isl_dump(isl_mat* mat) {isl_mat_dump(mat);}
