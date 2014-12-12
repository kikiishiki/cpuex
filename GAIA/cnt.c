#include <stdio.h>
#include "cnt.h"

long long inst_cnt = 0;

long long alu_cnt[32];
char *alu_name[32] = {
  "add", "sub", "shl", "shr", "sar",
  "and", "or" , "xor", ""   , ""   ,

  ""   , ""   , ""   , ""   , ""   ,
  ""   , ""   , ""   , ""   , ""   ,

  ""      , ""      , ""      , ""      , "cmpne" ,
  "cmpeq" , "cmplt" , "cmple" , "fcmpne", "fcmpeq",

  "fcmplt", "fcmple"
};

long long fpu_cnt[32];
char *fpu_name[32] = {
  "fadd" , "fsub" , "fmul" , "fdiv" , "finv" ,
  "fsqrt", "ftoi" , "itof" , "floor", ""     ,

  ""   , ""   , ""   , ""   , ""   ,
  ""   , ""   , ""   , ""   , ""   ,

  ""   , ""   , ""   , ""   , ""   ,
  ""   , ""   , ""   , ""   , ""   ,

  ""   , ""
};

long long ldl_cnt = 0;
long long ldh_cnt = 0;

long long st_cnt = 0;
long long ld_cnt = 0;

long long jl_cnt = 0;
long long jr_cnt = 0;
long long bne_cnt = 0;
long long beq_cnt = 0;

long long miss_cnt = 0;

void initialize_cnt() {
  int i;
  inst_cnt = 0;

  for (i=0; i<32; i++)
    alu_cnt[i] = 0;

  for (i=0; i<32; i++) 
    fpu_cnt[i] = 0;

  st_cnt = 0;
  ld_cnt = 0;

  jl_cnt = 0;
  jr_cnt = 0;
  bne_cnt = 0;
  beq_cnt = 0;

  miss_cnt = 0;
}

void print_cnt(FILE *fp) {
  int i;
  fputs("----------statistics----------\n", fp);
  for (i=0; i<32; i++) 
    if (alu_cnt[i] > 0) 
      fprintf(fp, "%s\t: %lld (%f%%)\n", alu_name[i], alu_cnt[i], alu_cnt[i] * 100.0 / inst_cnt);

  fputs("\n", fp);
  for (i=0; i<32; i++) 
    if (fpu_cnt[i] > 0) 
      fprintf(fp, "%s\t: %lld (%f%%)\n", fpu_name[i], fpu_cnt[i], fpu_cnt[i] * 100.0 / inst_cnt);
  
  fputs("\n", fp);
  fprintf(fp, "st\t: %lld (%f%%)\n", st_cnt, st_cnt * 100.0 / inst_cnt);
  fprintf(fp, "ld\t: %lld (%f%%)\n", ld_cnt, ld_cnt * 100.0 / inst_cnt);

  fputs("\n", fp);
  fprintf(fp, "jl\t: %lld (%f%%)\n", jl_cnt, jl_cnt * 100.0 / inst_cnt);
  fprintf(fp, "jr\t: %lld (%f%%)\n", jr_cnt, jr_cnt * 100.0 / inst_cnt);
  fprintf(fp, "bne\t: %lld (%f%%)\n", bne_cnt, bne_cnt * 100.0 / inst_cnt);
  fprintf(fp, "beq\t: %lld (%f%%)\n", beq_cnt, beq_cnt * 100.0 / inst_cnt);

  fputs("\n", fp);
  fprintf(fp, "cache miss  : %lld (%f%%)\n", miss_cnt, (ld_cnt == 0 ? 0 : miss_cnt*100.0 / ld_cnt));

}
