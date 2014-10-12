#ifndef _EXEC_MEM_H
#define _EXEC_MEM_H

#include <stdint.h>

void m_load(uint32_t rd, uint32_t rb, int16_t imm);
void m_store(uint32_t rd, uint32_t rb, int16_t imm);

#endif
