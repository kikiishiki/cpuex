#include "env.h"
#include <stdio.h>
#include <stdint.h>

uint32_t ireg[IREG_NUM];
uint32_t freg[FREG_NUM];
uint32_t creg;
uint32_t prog_cnt;
uint32_t mem[MEM_SIZE];
uint64_t cache[CACHE_SIZE];

uint32_t max_stack = MEM_SIZE;
uint32_t prog_size;

void initialize_env() {
  int i;
  for (i = 0; i < IREG_NUM; ++i) ireg[i] = 0;
  for (i = 0; i < FREG_NUM; ++i) freg[i] = 0;
  for (i = 0; i < CACHE_SIZE; i++) cache[i] = 0;
  ireg[STAC_PTR] = MEM_SIZE;
  ireg[BASE_PTR] = MEM_SIZE;
  prog_cnt = HEAP_ADDR + 1;
  mem[HEAP_ADDR] = HEAP_ADDR + prog_size + 1;
  
}

void print_env(FILE *fp) {
  int i, bp;
  for (i = 0; i < IREG_NUM; i++)
    fprintf(fp, "$r%2d = %11d (0x%08x)\n", i, ireg[i], ireg[i]);
  for (i = 0; i < FREG_NUM; i++)
    fprintf(fp, "$f%2d = %11d (0x%08x)\n", i, freg[i], freg[i]);
  fprintf(fp, "heap: %11d (0x%08x)\n", mem[HEAP_ADDR], mem[HEAP_ADDR]);
  fprintf(fp, "inst: %11d (0x%08x)\n", mem[prog_cnt], mem[prog_cnt]);
  fprintf(fp, "max stack: %d (0x%08x)\n", max_stack, max_stack);
  fprintf(fp, "stack trace: %d", prog_cnt);
  for (bp = ireg[BASE_PTR]; bp < MEM_SIZE; bp = mem[bp + 1])
    fprintf(fp, " <- %d", mem[bp] + 3);
  fprintf(fp, "\n");
}

