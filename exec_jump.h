#ifndef _EXEC_JUMP_H
#define _EXEC_JUMP_H

#include <stdint.h>

void beq(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm);
void ble(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm);

#endif
