//
// Created by SENSETIME\mupei on 2021/8/23.
//

#include "barvinok_case.h"

/*
	for (int i = 1; i <= n; ++i) {
	    for (int j = 1; j <= i; ++j) {
	        S
	    }
	}
 */
static inline void GetSetSpace(isl_ctx* ctx) {
	auto set = isl_set_read_from_str(ctx, "{[i, j]: 1 <= i <= 5 and 1 <= j <= i}");
	isl_dump(set);
	auto space = isl_set_get_space(set);
	isl_dump(space);
}

static inline void IntersectSet(isl_ctx* ctx) {
	auto set = isl_set_read_from_str(ctx, "[n] -> {[i, j]: 1 <= i <= n and 1 <= j <= i}");
	isl_dump(set);
	auto assign_n = isl_set_read_from_str(ctx, "[n] -> {: n = 5}");
	// 或者isl_set_intersect_params
	auto new_set = isl_set_flat_product(isl_set_copy(set), assign_n);
	isl_dump(new_set);
	auto is_equal = isl_set_is_equal(new_set, set); // false
	isl_assert(ctx, is_equal == isl_bool_false, isl_ctx_set_error(ctx, isl_error_abort));
}

static inline void GetStrideSpace(isl_ctx* ctx) {
	auto set = isl_set_read_from_str(ctx, "[n] -> { [i] : exists a : 1 <= i <= n and i = 1 + 3 a }");
	isl_dump(set);

	auto space = isl_set_get_space(set);
	isl_dump(space);
}

static inline void SetDomain(isl_ctx* ctx) {
	GetSetSpace(ctx);

	IntersectSet(ctx);

	GetStrideSpace(ctx);
}

void BarvinokCase() {
	struct isl_options *options = isl_options_new_with_defaults();
	assert(options);
	struct isl_ctx *ctx = isl_ctx_alloc_with_options(&isl_options_args, options);

	SetDomain(ctx);

	auto union_set_d = isl_union_set_read_from_str(
					ctx, "[n] -> {T[i] : 0 <= i < n; F[i,j] : 0 <= i < "
					     "n and 0 <= j < n - i;B[i] : 0 <= i < n}");
	isl_dump(union_set_d);
	auto union_pw_multi_aff_read = isl_union_pw_multi_aff_read_from_str(
					ctx, "{T[i] -> a[i];F[i,j] -> t[j];F[i,j] -> t[j + 1];B[i] -> t[i]}");
	isl_dump(union_pw_multi_aff_read);
	isl_dump(isl_union_pw_multi_aff_intersect_domain(union_pw_multi_aff_read,
	                                                 union_set_d));
}
