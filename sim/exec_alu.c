#include <stdint.h>
#include <stdio.h>
#include "exec_alu.h"

void add(uint32_t* reg, uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm)
{
  /* オーバーフローはとりあえず無視 */
  reg[rd] = reg[rs] + reg[rt] + imm;
}

void sub(uint32_t* reg, uint32_t rd, uint32_t rs, uint32_t rt)
{
  reg[rd] = reg[rs] - reg[rt];
}

void shift(uint32_t* reg, uint32_t rd, uint32_t rs, int16_t imm)
{
  /* immの値が-32〜31をはみ出す場合についてはとりあえず無視 */
  if (0 <= imm) {
    reg[rd] = reg[rs] << imm;
  } else {
    reg[rd] = (reg[rs] >> (-imm)) & (0x7fffffff >> (-imm-1));
  }

}

void fneg(uint32_t* reg, uint32_t rd, uint32_t rs)
{
  reg[rd] = reg[rs] ^ 0x80000000;
}
