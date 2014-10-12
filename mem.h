#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>

#define REG_NUM  16 
uint32_t reg[REG_NUM];

#define MEM_SIZE  (1 << 20)
uint32_t memory[MEM_SIZE];

#define INST_POINTER  (REG_NUM - 1)
#define BASE_POINTER  (MEM_SIZE - 1)
#define STAC_POINTER  (MEM_SIZE - 1)



void initialize_reg();
void print_reg();

#endif
