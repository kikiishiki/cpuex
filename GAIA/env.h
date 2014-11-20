#ifndef ENV_H_
#define ENV_H_

#include <stdint.h>

#define IREG_NUM   32
#define FREG_NUM   32 
#define MEM_SIZE   (1 << 20)
#define CACHE_SIZE (1 << 12)
#define STAC_PTR   30
#define BASE_PTR   31
#define INST_PTR   32
#define HEAP_ADDR  0x4000

extern uint32_t ireg[IREG_NUM];
extern uint32_t freg[FREG_NUM];
extern uint32_t prog_cnt;
extern uint32_t mem[MEM_SIZE];
extern uint64_t cache[CACHE_SIZE];
extern uint32_t max_stack;
extern uint32_t prog_size;

void initialize_env();
void print_env();

#endif
