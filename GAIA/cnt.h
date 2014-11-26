#ifndef _CNT_H
#define _CNT_H

extern long long inst_cnt;

extern long long alu_cnt[18];

extern long long fpu_cnt[34];

extern long long ld_cnt;
extern long long st_cnt;
extern long long fld_cnt;
extern long long fst_cnt;

extern long long jump_cnt[10];

extern long long miss_cnt;

void initialize_cnt();
void print_cnt();

#endif
