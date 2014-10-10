#include <stdint.h>
#include <stdio.h>
#include "runsim.h"
#include "exec_alu.h"

void sprit_code(uint32_t code, uint32_t* opcode, uint32_t* reg1, uint32_t* reg2, uint32_t* reg3, int16_t* imm);

void runsim(uint32_t code, uint32_t* reg)
{
  uint32_t opcode, reg1, reg2, reg3;
  int16_t imm;

  sprit_code(code, &opcode, &reg1, &reg2, &reg3, &imm);

  switch (opcode) {
    /* ALU */
  case 0x0:
    add(reg, reg1, reg2, reg3, imm);
    break;
  case 0x1:
    sub(reg, reg1, reg2, reg3);
    break;
  case 0x2:
    shift(reg, reg1, reg2, imm);
    break;
  case 0x3:
    fneg(reg, reg1, reg2);
    break;
  default:
    puts("undefined");
    break;
  }

}

void sprit_code(uint32_t code, uint32_t* opcode, uint32_t* reg1, uint32_t* reg2, uint32_t* reg3, int16_t* imm)
{
  *opcode = code >> 28;
  *reg1 = (code >> 24) & 0x0000000F;
  *reg2 = (code >> 20) & 0x0000000F;
  *reg3 = (code >> 16) & 0x0000000F;
  *imm  = code & 0x0000FFFF;
}

