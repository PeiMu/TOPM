//
// Created by SENSETIME\mupei on 2021/8/10.
//

#include "isl_dump.h"

void isl_dump(isl_set* set) {
	isl_set_dump(set);
}

void isl_dump(isl_aff* aff) {
	isl_aff_dump(aff);
}

void isl_dump(isl_id* id) {
	isl_id_dump(id);
}

void isl_dump(isl_schedule* schedule) {
	isl_schedule_dump(schedule);
}

void isl_dump(isl_space* space) {
	isl_space_dump(space);
}

void isl_dump(isl_union_map* union_map) {
	isl_union_map_dump(union_map);
}

void isl_dump(isl_val* val) {
	isl_val_dump(val);
}

void isl_dump(isl_ast_expr* expr) {
	isl_ast_expr_dump(expr);
}

void isl_dump(isl_id_list* id_list) {
	isl_id_list_dump(id_list);
}

void isl_dump(isl_multi_union_pw_aff *multi_union_pw_aff) {
	isl_multi_union_pw_aff_dump(multi_union_pw_aff);
}

void isl_dump(isl_multi_id *multi_id) {
	isl_multi_id_dump(multi_id);
}

void isl_dump(isl_multi_aff* multi_aff) {
	isl_multi_aff_dump(multi_aff);
}