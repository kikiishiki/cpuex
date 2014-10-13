#ifndef _EXEC_FPU_H
#define _EXEC_FPU_H

#include <stdint.h>

void fadd(uint32_t rd, uint32_t rs, uint32_t rt);
void fmul(uint32_t rd, uint32_t rs, uint32_t rt);

#endif
