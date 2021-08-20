#include "akg_code.h"

void BarvinokCase() {
  struct isl_options *options = isl_options_new_with_defaults();
  assert(options);
  struct isl_ctx *ctx = isl_ctx_alloc_with_options(&isl_options_args, options);
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

int main() {
  //  Driver();
  BarvinokCase();
  return 0;
}
