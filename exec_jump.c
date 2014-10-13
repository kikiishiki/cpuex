#include <stdio.h>
#include <stdint.h>

#include "mem.h"

void beq(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm)
{
  int32_t address;

  address = reg[rb] + imm;

  if (address < 0 || address >= MEM_SIZE) {
    printf("invalid address: %x\n", address);
  } else {
    if (reg[rs] == reg[rt]) {
      reg[INST_POINTER] = address - 1;
    }
  }
  
}



void ble(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm)
{
  int32_t address;

  address = reg[rb] + imm;

  if (address < 0 || address >= MEM_SIZE) {
    printf("invalid address: %x\n", address);
  } else {
    if (reg[rs] <= reg[rt]) {
      reg[INST_POINTER] = address - 1;
    }
  }
  
}
