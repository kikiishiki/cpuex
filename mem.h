#ifndef _MEM_H
#define _MEM_H

#define REG_NUM  16 
uint32_t reg[REG_NUM];

#define MEM_SIZE  (1 << 20)
uint32_t memory[MEM_SIZE];

void initialize_reg();
void print_reg();

#endif
