#ifndef _EXEC_ALU_H
#define _EXEC_ALU_H

#include <stdint.h>

void add(uint32_t* reg, uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm);
void sub(uint32_t* reg, uint32_t rd, uint32_t rs, uint32_t rt);
void shift(uint32_t* reg, uint32_t rd, uint32_t rs, int16_t imm);
void fneg(uint32_t* reg, uint32_t rd, uint32_t rs);

#endif
