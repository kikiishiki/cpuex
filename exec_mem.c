#include <stdio.h>
#include <stdint.h>

#include "exec_mem.h"
#include "mem.h"

void m_load(uint32_t rd, uint32_t rb, int16_t imm)
{
  int32_t address;

  address = reg[rb] + imm;
 
  if (address < 0 || address >= MEM_SIZE) {
    printf("invalid address: %x\n", address);
  } else {
    reg[rd] = memory[address];
  }

}

void m_store(uint32_t rd, uint32_t rb, int16_t imm)
{
  int32_t address;

  address = reg[rb] + imm;

  if (address < 0 || address >= MEM_SIZE) {
    printf("invalid address: %x\n", address);
  } else {
    memory[address] = reg[rd];
  }

}
