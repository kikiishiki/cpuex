#include <stdio.h>
#include "cnt.h"

long long inst_cnt = 0;

long long alu_cnt[18];
char *alu_name[18] = {
  "add",
  "sub",
  "shl",
  "shr",
  "sar",
  "and",
  "or",
  "not",
  "xor",
  "cat",
  "cmpne",
  "cmpeq",
  "cmplt",
  "cmple",
  "ldl",
  "ldh"
};

long long fpu_cnt[34];
char *fpu_name[34] = {
  "fadd",
  "fsub",
  "fmul",
  "fdiv",
  "finv",
  "fsqrt",
  "ftoi",
  "itof",
  "floor",
  "ffma",
  "fcat",
  "fand",
  "for",
  "fxor",
  "fnot",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "fcmpne",
  "fcmpeq",
  "fcmplt",
  "fcmple",
  "fldl",
  "fldh"
};

long long ld_cnt = 0;
long long st_cnt = 0;
long long fld_cnt = 0;
long long fst_cnt = 0;

long long jump_cnt[10];
char *jump_name[10] = {
  "bne",
  "beq",
  "blt",
  "ble",
  "fbne",
  "fbeq",
  "fblt",
  "fble",
  "jl",
  "jr"
};

long long miss_cnt = 0;

void initialize_cnt() {
  int i;
  inst_cnt = 0;

  for (i=0; i<18; i++)
    alu_cnt[i] = 0;

  for (i=0; i<34; i++) 
    fpu_cnt[i] = 0;

  ld_cnt = 0;
  st_cnt = 0;
  fld_cnt = 0;
  fst_cnt = 0;

  for (i=0; i<10; i++)
    jump_cnt[i] = 0;

  miss_cnt = 0;
}

void print_cnt(FILE *fp) {
  int i;
  fputs("----------statistics----------\n", fp);
  for (i=0; i<18; i++) 
    if (alu_cnt[i] > 0) 
      fprintf(fp, "%s\t: %lld (%f%%)\n", alu_name[i], alu_cnt[i], alu_cnt[i] * 100.0 / inst_cnt);

  fputs("\n", fp);
  for (i=0; i<34; i++) 
    if (fpu_cnt[i] > 0) 
      fprintf(fp, "%s\t: %lld (%f%%)\n", fpu_name[i], fpu_cnt[i], fpu_cnt[i] * 100.0 / inst_cnt);
  
  fputs("\n", fp);
  fprintf(fp, "ld\t: %lld (%f%%)\n", ld_cnt, ld_cnt * 100.0 / inst_cnt);
  fprintf(fp, "st\t: %lld (%f%%)\n", st_cnt, st_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fld\t: %lld (%f%%)\n", fld_cnt, fld_cnt * 100.0 / inst_cnt);
  fprintf(fp, "fst\t: %lld (%f%%)\n", fst_cnt, fst_cnt * 100.0 / inst_cnt);

  fputs("\n", fp);
  for (i=0; i<10; i++) 
    if (jump_cnt[i] > 0) 
      fprintf(fp, "%s\t: %lld (%f%%)\n", jump_name[i], jump_cnt[i], jump_cnt[i] * 100.0 / inst_cnt);

  fputs("\n", fp);
  fprintf(fp, "cache miss  : %lld (%f%%)\n", miss_cnt, (ld_cnt+fld_cnt == 0 ? 0 : miss_cnt*100.0 / (ld_cnt + fld_cnt)));

}
