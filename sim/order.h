#ifndef _ITYPE_H
#define _ITYPE_H

void run_itype(uint32_t opcode, uint32_t* reg);
void sprit_code_itype(uint32_t opcode, int* op, int* rs, int* rt, int* immediate);
void addi(uint32_t* reg, int rs, int rt, int immediate);

#endif
