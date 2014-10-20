#ifndef ENV_H_
#define ENV_H_

#include <stdint.h>

#define REG_NUM   16 
#define MEM_SIZE  (1 << 20)
#define STAC_PTR  13
#define BASE_PTR  14
#define INST_PTR  15
#define HEAP_ADDR 0x4000

extern uint32_t reg[REG_NUM];
extern uint32_t mem[MEM_SIZE];
extern uint32_t max_stack;

void initialize_env();
void print_env();

#endif
