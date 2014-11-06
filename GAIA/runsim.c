#include "cnt.h"
#include "env.h"
#include "util.h"
#include "io.h"
#include "runsim.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern uint32_t fmul();
union Ui_f { uint32_t n; float f; };

void add(uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm) {
  /* オーバーフローはとりあえず無視 */
  ireg[rd] = ireg[rs] + ireg[rt] + imm;
}

void sub(uint32_t rd, uint32_t rs, uint32_t rt) {
  ireg[rd] = ireg[rs] - ireg[rt];
}

void shift(uint32_t rd, uint32_t rs, uint32_t rt, int16_t imm) {
  int x;
  x = ireg[rt] + imm;
  /* xの値が-32〜32をはみ出す場合は0 */
       if (  0 <= x && x < 32) ireg[rd] = ireg[rs] << x;
  else if (-32 <  x && x <  0) ireg[rd] = ireg[rs] >> (-x);
  else                         ireg[rd] = 0;
}

void fneg(uint32_t rd, uint32_t rs) {
  if (freg[rs] != 0) freg[rd] = freg[rs] ^ 0x80000000; // -0は回避
  else               freg[rd] = 0;
}

void fadd(uint32_t rd, uint32_t rs, uint32_t rt) {
  union Ui_f ui_fd, ui_fs, ui_ft;
  ui_fs.n = freg[rs];
  ui_ft.n = freg[rt];
  ui_fd.f = ui_fs.f + ui_ft.f;
  if (ui_fd.n == 0x80000000) ui_fd.n = 0; // -0
  freg[rd] = ui_fd.n;
}

void fmul_(uint32_t rd, uint32_t rs, uint32_t rt) {
  freg[rd] = fmul(freg[rs], freg[rt]);
}

void finv(uint32_t rd, uint32_t rs) {
  /* 入力が0の際の処理などはしていない
     出力がinfになっているかもしれない */
  union Ui_f ui_fd, ui_fs;
  ui_fs.n = freg[rs];
  ui_fd.f = 1.0 / ui_fs.f;
  if (ui_fd.n == 0x80000000) ui_fd.n = 0; // -0
  freg[rd] = ui_fd.n;
}

void fsqrt(uint32_t rd, uint32_t rs) {
  /* 入力が負の際の処理などはしていない */
  union Ui_f ui_fd, ui_fs;
  ui_fs.n = freg[rs];
  ui_fd.f = sqrtf(ui_fs.f);
  freg[rd] = ui_fd.n;
}

void m_load(uint32_t rd, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("load: invalid address: %x\n", address);
  ireg[rd] = mem[address];
}

void m_store(uint32_t rs, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("store: invalid address: %x\n", address);
  mem[address] = ireg[rs];
}

void beq(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("beq: invalid address: %x\n", address);
  if (ireg[rs] == ireg[rt])
    prog_cnt = address - 1;
}

void ble(uint32_t rs, uint32_t rt, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("ble: invalid address: %x\n", address);
  if ((int32_t)ireg[rs] <= (int32_t)ireg[rt])
    prog_cnt = address - 1;
}

void split_code(uint32_t code, uint32_t* opcode, uint32_t* reg1, uint32_t* reg2, uint32_t* reg3, int16_t* imm) {
  *opcode = code >> 28;
  *reg1 = (code >> 24) & 0xf;
  *reg2 = (code >> 20) & 0xf;
  *reg3 = (code >> 16) & 0xf;
  *imm  = code & 0xffff;
}

void error_check()
{
  if (ireg[0] != 0 || freg[0] != 0)
    error("$0 is immutable");
  if (mem[HEAP_ADDR] > ireg[STAC_PTR])
    error("stack overflow");
}

void runsim(uint32_t code)
{
  uint32_t opcode, reg1, reg2, reg3;
  int16_t imm;

  split_code(code, &opcode, &reg1, &reg2, &reg3, &imm);

  switch (opcode) {
    /* ALU */
    case 0x0: add(reg1, reg2, reg3, imm);   add_cnt++;   break;
    case 0x1: sub(reg1, reg2, reg3);        sub_cnt++;   break;
    case 0x2: shift(reg1, reg2, reg3, imm); shift_cnt++; break;
    case 0x3: fneg(reg1, reg2);             fneg_cnt++;  break;
    /* FPU */
    case 0x4: fadd(reg1, reg2, reg3); fadd_cnt++;  break;
    case 0x5: fmul_(reg1, reg2, reg3); fmul_cnt++;  break;
    case 0x6: finv(reg1, reg2);       finv_cnt++;  break;
    case 0x7: fsqrt(reg1, reg2);      fsqrt_cnt++; break;
    /* MEMORY */
    case 0x8: m_load(reg1, reg3, imm);  load_cnt++;  break;
    case 0x9: m_store(reg2, reg3, imm); store_cnt++; break;
    /* IO */
    case 0xa: io_read(reg1);  read_cnt++;  break;
    case 0xb: io_write(reg2); write_cnt++; break;
    /* Branch */
    case 0xc: beq(reg1, reg2, reg3, imm); beq_cnt++; break;
    case 0xd: ble(reg1, reg2, reg3, imm); ble_cnt++; break;
    /* Error */
    default:  error("invalid opcode: %08x", code); break;
  }
  
  error_check();
  
  if (ireg[STAC_PTR] < max_stack) max_stack = ireg[STAC_PTR];
}

