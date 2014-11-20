#ifndef _CNT_H
#define _CNT_H

extern long long inst_cnt;

extern long long add_cnt;
extern long long sub_cnt;
extern long long shl_cnt;
extern long long shr_cnt;
extern long long sar_cnt;
extern long long and_cnt;
extern long long or_cnt;
extern long long not_cnt;
extern long long xor_cnt;
extern long long cat_cnt;
extern long long cmpne_cnt;
extern long long cmpeq_cnt;
extern long long cmplt_cnt;
extern long long cmple_cnt;
extern long long ldl_cnt;
extern long long ldh_cnt;

extern long long fadd_cnt;
extern long long fsub_cnt;
extern long long fmul_cnt;
extern long long fdiv_cnt;
extern long long finv_cnt;
extern long long fsqrt_cnt;
extern long long ftoi_cnt;
extern long long itof_cnt;
extern long long floor_cnt;
extern long long ffma_cnt;
extern long long fcat_cnt;
extern long long fand_cnt;
extern long long for_cnt;
extern long long fxor_cnt;
extern long long fnot_cnt;
extern long long fcmpne_cnt;
extern long long fcmpeq_cnt;
extern long long fcmplt_cnt;
extern long long fcmple_cnt;
extern long long fldl_cnt;
extern long long fldh_cnt;

extern long long ld_cnt;
extern long long st_cnt;
extern long long fld_cnt;
extern long long fst_cnt;

extern long long bne_cnt;
extern long long beq_cnt;
extern long long blt_cnt;
extern long long ble_cnt;
extern long long fbne_cnt;
extern long long fbeq_cnt;
extern long long fblt_cnt;
extern long long fble_cnt;

extern long long jl_cnt;
extern long long jr_cnt;

extern long long miss_cnt;

void initialize_cnt();
void print_cnt();

#endif
