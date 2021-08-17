//
// Created by SENSETIME\mupei on 2021/8/11.
//

#include "test_expr_compute.h"

/**
 * @brief test basic ast, which from isl_test.c: test_ast
 * @param ctx
 * @return
 */
int test_expr_compute(isl_ctx *ctx) {
	isl_ast_expr *expr, *expr1, *expr2, *expr3;
	char *str;
	int ok, equal;

	expr1 = isl_ast_expr_from_id(isl_id_alloc(ctx, "A", nullptr));
	isl_dump(expr1);
	expr2 = isl_ast_expr_from_id(isl_id_alloc(ctx, "B", nullptr));
	isl_dump(expr2);
	expr = isl_ast_expr_add(expr1, expr2);
	isl_dump(expr);

	expr2 = isl_ast_expr_copy(expr);
	expr = isl_ast_expr_neg(expr);
	isl_dump(expr);
	expr2 = isl_ast_expr_neg(expr2);
	equal = isl_ast_expr_is_equal(expr, expr2);

	str = isl_ast_expr_to_C_str(expr);
	printf("\n%s\n", str);
	ok = str ? !strcmp(str, "-(A + B)") : -1;
	free(str);
	isl_ast_expr_free(expr);
	isl_ast_expr_free(expr2);

	if (ok < 0 || equal < 0)
		return -1;
	if (!equal)
		isl_die(ctx, isl_error_unknown, "equal expressions not considered equal",
	return -1);
	if (!ok)
		isl_die(ctx, isl_error_unknown, "isl_ast_expr printed incorrectly",
	return -1);

	expr1 = isl_ast_expr_from_val(isl_val_read_from_str(ctx, "1"));
	expr2 = isl_ast_expr_from_val(isl_val_int_from_si(ctx, -3));
	expr3 = isl_ast_expr_from_val(isl_val_int_from_ui(ctx, 5));
	expr = isl_ast_expr_add(expr1, expr2);
	expr = isl_ast_expr_sub(expr, expr3);
	str = isl_ast_expr_to_C_str(expr);
	ok = str ? !strcmp(str, "1 + -3 - 5") : -1;
	free(str);
	isl_ast_expr_free(expr);

	if (ok < 0)
		return -1;
	if (!ok)
		isl_die(ctx, isl_error_unknown, "isl_ast_expr printed incorrectly",
	return -1);

	expr1 = isl_ast_expr_from_id(isl_id_alloc(ctx, "A", nullptr));
	expr2 = isl_ast_expr_from_id(isl_id_alloc(ctx, "B", nullptr));
	expr = isl_ast_expr_add(expr1, expr2);
	expr3 = isl_ast_expr_from_id(isl_id_alloc(ctx, "C", nullptr));
	expr = isl_ast_expr_sub(expr3, expr);
	str = isl_ast_expr_to_C_str(expr);
	ok = str ? !strcmp(str, "C - (A + B)") : -1;
	free(str);
	isl_ast_expr_free(expr);

	if (ok < 0)
		return -1;
	if (!ok)
		isl_die(ctx, isl_error_unknown, "isl_ast_expr printed incorrectly",
	return -1);

	return 0;
}