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

void shift(uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm)
{
  int shift_;

  shift_ = reg[rt] + imm;
  /* immの値が-32〜32をはみ出す場合は0 */
  if (0 <= shift_ && shift_ <= 32) {
    reg[rd] = reg[rs] << imm;
  } else if (-32 <= shift_ && shift_ < 0) {
    reg[rd] = reg[rs] >> (-imm);
  } else {
    reg[rd] = 0;
  }

}

void fneg(uint32_t rd, uint32_t rs)
{
  if (reg[rs] != 0) { // -0は回避
    reg[rd] = reg[rs] ^ 0x80000000;
  }
}
