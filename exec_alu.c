#include <stdint.h>
#include <stdio.h>

#include "exec_alu.h"
#include "mem.h"

void add(uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm)
{
  /* オーバーフローはとりあえず無視 */
  reg[rd] = reg[rs] + reg[rt] + (int32_t)imm;
}

void sub(uint32_t rd, uint32_t rs, uint32_t rt)
{
  reg[rd] = reg[rs] - reg[rt];
}

void shift(uint32_t rd, uint32_t rs, int16_t imm)
{
  /* immの値が-32〜31をはみ出す場合についてはとりあえず無視 */
  if (imm >= 0) {
    reg[rd] = reg[rs] << imm;
  } else {
    reg[rd] = reg[rs] >> (-imm + 1);
  }

}

void fneg(uint32_t rd, uint32_t rs)
{
  if (reg[rs] != 0) { // -0は回避
    reg[rd] = reg[rs] ^ 0x80000000;
  }
}
