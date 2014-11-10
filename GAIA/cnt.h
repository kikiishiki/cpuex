#ifndef _CNT_H
#define _CNT_H

extern long long inst_cnt;

extern long long add_cnt;
extern long long sub_cnt;
extern long long shift_cnt;
extern long long fneg_cnt;
extern long long fadd_cnt;
extern long long fmul_cnt;
extern long long finv_cnt;
extern long long fsqrt_cnt;
extern long long ld_cnt;
extern long long st_cnt;
extern long long read_cnt;
extern long long write_cnt;
extern long long beq_cnt;
extern long long ble_cnt;

void initialize_cnt();
void print_cnt();

#endif
