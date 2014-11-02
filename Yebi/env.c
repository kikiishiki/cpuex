#include "env.h"
#include <stdio.h>
#include <stdint.h>

uint32_t reg[REG_NUM];
uint32_t mem[MEM_SIZE];

uint32_t max_stack = MEM_SIZE;
uint32_t prog_size;

void initialize_env() {
  int i;
  for (i = 0; i < REG_NUM; ++i) reg[i] = 0;
  reg[STAC_PTR] = MEM_SIZE;
  reg[BASE_PTR] = MEM_SIZE;
  reg[INST_PTR] = HEAP_ADDR + 1;
  mem[HEAP_ADDR] = HEAP_ADDR + prog_size + 1;
  

}

void print_env(FILE *fp) {
  int i, bp;
  for (i = 0; i < REG_NUM; i++)
    fprintf(fp, "$%2d = %11d (0x%08x)\n", i, reg[i], reg[i]);
  fprintf(fp, "heap: %11d (0x%08x)\n", mem[HEAP_ADDR], mem[HEAP_ADDR]);
  fprintf(fp, "inst: %11d (0x%08x)\n", mem[INST_PTR], mem[INST_PTR]);
  fprintf(fp, "max stack: %d (0x%08x)\n", max_stack, max_stack);
  fprintf(fp, "stack trace: %d", reg[INST_PTR]);
  for (bp = reg[BASE_PTR]; bp < MEM_SIZE; bp = mem[bp + 1])
    fprintf(fp, " <- %d", mem[bp] + 3);
  fprintf(fp, "\n");
}

