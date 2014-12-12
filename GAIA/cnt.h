#ifndef _CNT_H
#define _CNT_H

extern long long inst_cnt;

extern long long alu_cnt[32];

extern long long fpu_cnt[32];

extern long long ldl_cnt;
extern long long ldh_cnt;

extern long long st_cnt;
extern long long ld_cnt;

extern long long jl_cnt;
extern long long jr_cnt;
extern long long bne_cnt;
extern long long beq_cnt;

extern long long miss_cnt;

void initialize_cnt();
void print_cnt();

#endif
