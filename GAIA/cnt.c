#include <stdio.h>
#include "cnt.h"

long long inst_cnt = 0;

long long add_cnt = 0;
long long sub_cnt = 0;
long long shl_cnt = 0;
long long shr_cnt = 0;
long long sar_cnt = 0;
long long and_cnt = 0;
long long or_cnt = 0;
long long not_cnt = 0;
long long xor_cnt = 0;
long long cat_cnt = 0;
long long cmpne_cnt = 0;
long long cmpeq_cnt = 0;
long long cmplt_cnt = 0;
long long cmple_cnt = 0;
long long ldl_cnt = 0;
long long ldh_cnt = 0;

long long fadd_cnt = 0;
long long fsub_cnt = 0;
long long fmul_cnt = 0;
long long fdiv_cnt = 0;
long long finv_cnt = 0;
long long fsqrt_cnt = 0;
long long ftoi_cnt = 0;
long long itof_cnt = 0;
long long floor_cnt = 0;
long long ffma_cnt = 0;
long long fcat_cnt = 0;
long long fcmpne_cnt = 0;
long long fcmpeq_cnt = 0;
long long fcmplt_cnt = 0;
long long fcmple_cnt = 0;
long long fldl_cnt = 0;
long long fldh_cnt = 0;

long long ld_cnt = 0;
long long st_cnt = 0;
long long fld_cnt = 0;
long long fst_cnt = 0;

long long bne_cnt = 0;
long long beq_cnt = 0;
long long blt_cnt = 0;
long long ble_cnt = 0;
long long fbne_cnt = 0;
long long fbeq_cnt = 0;
long long fblt_cnt = 0;
long long fble_cnt = 0;

long long jl_cnt = 0;
long long jr_cnt = 0;

long long miss_cnt = 0;

void initialize_cnt() {
  inst_cnt = 0;

  add_cnt = 0;
  sub_cnt = 0;
  shl_cnt = 0;
  shr_cnt = 0;
  sar_cnt = 0;
  and_cnt = 0;
  or_cnt = 0;
  not_cnt = 0;
  xor_cnt = 0;
  cat_cnt = 0;
  cmpne_cnt = 0;
  cmpeq_cnt = 0;
  cmplt_cnt = 0;
  cmple_cnt = 0;
  ldl_cnt = 0;
  ldh_cnt = 0;

  fadd_cnt = 0;
  fsub_cnt = 0;
  fmul_cnt = 0;
  fdiv_cnt = 0;
  finv_cnt = 0;
  fsqrt_cnt = 0;
  ftoi_cnt = 0;
  itof_cnt = 0;
  floor_cnt = 0;
  ffma_cnt = 0;
  fcat_cnt = 0;
  fcmpne_cnt = 0;
  fcmpeq_cnt = 0;
  fcmplt_cnt = 0;
  fcmple_cnt = 0;
  fldl_cnt = 0;
  fldh_cnt = 0;

  ld_cnt = 0;
  st_cnt = 0;
  fld_cnt = 0;
  fst_cnt = 0;

  bne_cnt = 0;
  beq_cnt = 0;
  blt_cnt = 0;
  ble_cnt = 0;
  fbne_cnt = 0;
  fbeq_cnt = 0;
  fblt_cnt = 0;
  fble_cnt = 0;

  jl_cnt = 0;
  jr_cnt = 0;

  miss_cnt = 0;
}

void print_cnt(FILE *fp) {
  fputs("----------statistics----------\n", fp);
  /*
  fprintf(fp, "add  : %lld (%f%%)\n", add_cnt, add_cnt * 100.0 / inst_cnt);
  fprintf(fp, "sub  : %lld (%f%%)\n", sub_cnt, sub_cnt * 100.0 / inst_cnt);
  fprintf(fp, "shift: %lld (%f%%)\n", shift_cnt, shift_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fneg : %lld (%f%%)\n", fneg_cnt, fneg_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fadd : %lld (%f%%)\n", fadd_cnt, fadd_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fmul : %lld (%f%%)\n", fmul_cnt, fmul_cnt * 100.0 / inst_cnt);
  fprintf(fp, "finv : %lld (%f%%)\n", finv_cnt, finv_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fsqrt: %lld (%f%%)\n", fsqrt_cnt, fsqrt_cnt * 100.0 / inst_cnt);
  fprintf(fp, "ld   : %lld (%f%%)\n", ld_cnt, ld_cnt * 100.0 / inst_cnt);
  fprintf(fp, "st   : %lld (%f%%)\n", st_cnt, st_cnt * 100.0 / inst_cnt);
  fprintf(fp, "read : %lld (%f%%)\n", read_cnt, read_cnt * 100.0 / inst_cnt);
  fprintf(fp, "write: %lld (%f%%)\n", write_cnt, write_cnt * 100.0 / inst_cnt);
  fprintf(fp, "beq  : %lld (%f%%)\n", beq_cnt, beq_cnt * 100.0 / inst_cnt);
  fprintf(fp, "ble  : %lld (%f%%)\n", ble_cnt, ble_cnt * 100.0 / inst_cnt);
  fputs("", fp);
  fprintf(fp, "cache miss  : %lld (%f%%)\n", miss_cnt, (ld_cnt+ldf_cnt == 0 ? 0 : miss_cnt*100.0 / (ld_cnt + ldf_cnt)));
  */
}
