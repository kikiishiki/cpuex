#include <stdio.h>
#include "cnt.h"

long long inst_cnt = 0;

long long add_cnt = 0;
long long sub_cnt = 0;
long long shift_cnt = 0;
long long fneg_cnt = 0;
long long fadd_cnt = 0;
long long fmul_cnt = 0;
long long finv_cnt = 0;
long long fsqrt_cnt = 0;
long long load_cnt = 0;
long long store_cnt = 0;
long long read_cnt = 0;
long long write_cnt = 0;
long long beq_cnt = 0;
long long ble_cnt = 0;

void initialize_cnt() {
  inst_cnt = 0;

  add_cnt = 0;
  sub_cnt = 0;
  shift_cnt = 0;
  fneg_cnt = 0;
  fadd_cnt = 0;
  fmul_cnt = 0;
  finv_cnt = 0;
  fsqrt_cnt = 0;
  load_cnt = 0;
  store_cnt = 0;
  read_cnt = 0;
  write_cnt = 0;
  beq_cnt = 0;
  ble_cnt = 0;
}

void print_cnt(FILE *fp) {
  fputs("----------statistics----------\n", fp);
  fprintf(fp, "add  : %lld (%f%%)\n", add_cnt, add_cnt * 100.0 / inst_cnt);
  fprintf(fp, "sub  : %lld (%f%%)\n", sub_cnt, sub_cnt * 100.0 / inst_cnt);
  fprintf(fp, "shift: %lld (%f%%)\n", shift_cnt, shift_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fneg : %lld (%f%%)\n", fneg_cnt, fneg_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fadd : %lld (%f%%)\n", fadd_cnt, fadd_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fmul : %lld (%f%%)\n", fmul_cnt, fmul_cnt * 100.0 / inst_cnt);
  fprintf(fp, "finv : %lld (%f%%)\n", finv_cnt, finv_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fsqrt: %lld (%f%%)\n", fsqrt_cnt, fsqrt_cnt * 100.0 / inst_cnt);
  fprintf(fp, "load : %lld (%f%%)\n", load_cnt, load_cnt * 100.0 / inst_cnt);
  fprintf(fp, "store: %lld (%f%%)\n", store_cnt, store_cnt * 100.0 / inst_cnt);
  fprintf(fp, "read : %lld (%f%%)\n", read_cnt, read_cnt * 100.0 / inst_cnt);
  fprintf(fp, "write: %lld (%f%%)\n", write_cnt, write_cnt * 100.0 / inst_cnt);
  fprintf(fp, "beq  : %lld (%f%%)\n", beq_cnt, beq_cnt * 100.0 / inst_cnt);
  fprintf(fp, "ble  : %lld (%f%%)\n", ble_cnt, ble_cnt * 100.0 / inst_cnt);
}
