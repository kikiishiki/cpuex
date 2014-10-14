#include <stdint.h>
#include <stdio.h>
#include "runsim.h"
#include "exec_alu.h"
#include "exec_fpu.h"
#include "exec_mem.h"
#include "exec_jump.h"
#include "exec_IO.h"
#include "mem.h"

void sprit_code(uint32_t code, uint32_t* opcode, uint32_t* reg1, uint32_t* reg2, uint32_t* reg3, int16_t* imm);

void runsim(uint32_t code)
{
  uint32_t opcode, reg1, reg2, reg3;
  int16_t imm;

  sprit_code(code, &opcode, &reg1, &reg2, &reg3, &imm);

  switch (opcode) {
    /* ALU */
  case 0x0:
    add(reg1, reg2, reg3, imm);
    break;
  case 0x1:
    sub(reg1, reg2, reg3);
    break;
  case 0x2:
    shift(reg1, reg2, imm);
    break;
  case 0x3:
    fneg(reg1, reg2);
    break;
    /* FPU */
  case 0x4:
    fadd(reg1, reg2, reg3);
    break;
  case 0x5:
    fmul(reg1, reg2, reg3);
    break;
    /* MEMORY */
  case 0x8:
    m_load(reg1, reg3, imm);
    break;
  case 0x9:
    m_store(reg2, reg3, imm);
    break;
    /* IO */
  case 0xa:
    io_read(reg1);
    break;
  case 0xb:
    io_write(reg2);
    break;
    /* Branch */
  case 0xc:
    beq(reg1, reg2, reg3, imm);
    break;
  case 0xd:
    ble(reg1, reg2, reg3, imm);
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

