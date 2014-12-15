#include "cnt.h"
#include "env.h"
#include "util.h"
#include "ldst.h"
#include "io.h"
#include "runsim.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern uint32_t fmul();
extern uint32_t load();
extern void store();
union Ui_f { uint32_t n; float f; };

void add(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  /* オーバーフローはとりあえず無視 */
  ireg[rx] = ireg[ra] + ireg[rb] + imm;
}

void sub(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] - ireg[rb] - imm;
}

void shl(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] << (ireg[rb] + imm);
}

void shr(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] >> (ireg[rb] + imm);
}

void sar(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = (ireg[ra] >> (ireg[rb] + imm)) | (0xffffffff << (32 - ireg[rb] - imm));
}

void and(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] & ireg[rb] & imm;
}

void or(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] | ireg[rb] | imm;
}

void xor(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] ^ ireg[rb] ^ imm;
}

void cmpne(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] != ireg[rb] + imm ? 1 : 0;
}

void cmpeq(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] == ireg[rb] + imm ? 1 : 0;
}

void cmplt(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] < ireg[rb] + imm ? 1 : 0;
}

void cmple(uint32_t rx, uint32_t ra, uint32_t rb, int16_t imm) {
  ireg[rx] = ireg[ra] <= ireg[rb] + imm ? 1 : 0;
}

void fcmpne(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = freg[fa] != freg[fb] ? 1 : 0;
}

void fcmpeq(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = freg[fa] == freg[fb] ? 1 : 0;
}

void fcmplt(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  freg[fx] = ui_fa.f < ui_fb.f ? 1 : 0;
}

void fcmple(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  freg[fx] = ui_fa.f <= ui_fb.f ? 1 : 0;
}

int sign_mode = 0;
uint32_t operate_sign_bit(uint32_t val) {
  if (val != 0) {
    if      (sign_mode == 1) val ^= 0x80000000;
    else if (sign_mode == 2) val |= 0x80000000;
    else if (sign_mode == 3) val &= 0x00000000;
  }
  return val;
}

void fadd(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fx, ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  ui_fx.f = ui_fa.f + ui_fb.f;
  if (ui_fx.n == 0x80000000) ui_fx.n = 0; // -0
  freg[fx] = operate_sign_bit(ui_fx.n);
}

void fsub(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fx, ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  ui_fx.f = ui_fa.f - ui_fb.f;
  if (ui_fx.n == 0x80000000) ui_fx.n = 0; // -0
  freg[fx] = operate_sign_bit(ui_fx.n);
}

void fmul_(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = operate_sign_bit( fmul(freg[fa], freg[fb]) );
}

void fdiv(uint32_t fx, uint32_t fa, uint32_t fb) {
  /* 入力が0にあまりに近い際の処理などはしていない
     出力がinfになっているかもしれない */
  union Ui_f ui_fx, ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  if (ui_fb.n == 0)
    error("fdiv: div by Zero");
  ui_fx.f = ui_fa.f / ui_fb.f;
  if (ui_fx.n == 0x80000000) ui_fx.n = 0; // -0
  freg[fx] = operate_sign_bit(ui_fx.n);
}

void finv(uint32_t fx, uint32_t fa) {
  /* 入力が0にあまりに近い際の処理などはしていない
     出力がinfになっているかもしれない */
  union Ui_f ui_fx, ui_fa;
  ui_fa.n = freg[fa];
  if (ui_fa.n == 0)
    error("finv: div by Zero");
  ui_fx.f = 1.0 / ui_fa.f;
  if (ui_fx.n == 0x80000000) ui_fx.n = 0; // -0
  freg[fx] = operate_sign_bit(ui_fx.n);
}

void fsqrt(uint32_t fx, uint32_t fa) {
  /* 入力が負の際の処理などはしていない */
  union Ui_f ui_fx, ui_fa;
  ui_fa.n = freg[fa];
  ui_fx.f = sqrtf(ui_fa.f);
  freg[fx] = operate_sign_bit(ui_fx.n);
}

void ftoi(uint32_t fx, uint32_t fa) {
}

void itof(uint32_t fx, uint32_t fa) {
}

void floor_(uint32_t fx, uint32_t fa) {
}

void ldl(uint32_t rx, uint32_t ra, int16_t imm) {
  ireg[rx] = (ireg[ra] & 0xffff0000) | ((uint32_t)imm & 0x0000ffff);
}

void ldh(uint32_t rx, uint32_t ra, int16_t imm) {
  ireg[rx] = (((uint32_t)imm << 16) & 0xffff0000) | (ireg[ra] & 0x0000ffff);
}


void ld(uint32_t rx, uint32_t ra, int16_t disp) {
  int32_t address;
  address = ireg[ra] + disp*4;
  if (address < 0 || address >= MEM_SIZE)
    error("load: invalid address: %x\n", address);
  ireg[rx] = load(address);
}

void st(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = ireg[rb] + disp*4;
  if (address < 0 || address >= MEM_SIZE)
    error("store: invalid address: %x\n", address);
  store(address,ireg[ra]);
}

void jl(uint32_t rx, int16_t disp) {
  int32_t address;
  address = prog_cnt + 4 + disp*4;
  if (address < 0 || address >= MEM_SIZE)
    error("jl: invalid address: %x\n", address);
  ireg[rx] = prog_cnt + 4;
  prog_cnt = address;
}

void jr(uint32_t ra) {
  prog_cnt = ireg[ra];
}

void bne(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + 4 + disp*4;
  if (address < 0 || address >= MEM_SIZE)
    error("bne: invalid address: %x\n", address);
  if (ireg[ra] != ireg[rb])
    prog_cnt = address;
}

void beq(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + 4 + disp*4;
  if (address < 0 || address >= MEM_SIZE)
    error("beq: invalid address: %x\n", address);
  if (ireg[ra] == ireg[rb])
    prog_cnt = address;
}

/* ここからrunsimの本体部分 */
void split_alu(uint32_t code, uint32_t *regx, uint32_t *rega, uint32_t *regb, int16_t *imm, uint32_t *tag) {
  *regx = (code >> 23) & 0x1f;
  *rega = (code >> 18) & 0x1f;
  *regb = (code >> 13) & 0x1f;
  *imm  = (code >> 5)  & 0xff;
  if ((*imm >> 7) == 1)
    *imm |= 0xff00; // 符号拡張
  *tag  = code & 0x1f;
}

void split_fpu(uint32_t code, uint32_t *regx, uint32_t *rega, uint32_t *regb, uint32_t *s, uint32_t *tag) {
  *regx = (code >> 23) & 0x1f;
  *rega = (code >> 18) & 0x1f;
  *regb = (code >> 13) & 0x1f;
  *s    = (code >> 5)  & 0x3;
  *tag  = code & 0x1f;
}

void split_mem(uint32_t code, uint32_t *regx, uint32_t *rega, uint32_t *pred, int16_t *disp) { 
  *regx = (code >> 23) & 0x1f;
  *rega = (code >> 18) & 0x1f;
  *pred  = (code >> 16) & 0x3;
  *disp  = code & 0xffff;
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
  uint32_t opcode = 0, regx = 0, rega = 0, regb = 0, s = 0, tag = 0, pred = 0;
  int16_t imm = 0, disp = 0;

  opcode = code >> 28;

  switch (opcode) {
  case 0x0: split_alu(code, &regx, &rega, &regb, &imm, &tag); break;
  case 0x1: split_fpu(code, &regx, &rega, &regb, &s, &tag); break;
  default:  split_mem(code, &rega, &regb, &pred, &disp); break;
  }

  switch (opcode) {
    /* ALU */
  case 0x0:
    alu_cnt[tag]++;
    switch (tag) {
    case 0x00: add(regx, rega, regb, imm); break;
    case 0x01: sub(regx, rega, regb, imm); break;
    case 0x02: shl(regx, rega, regb, imm); break;
    case 0x03: shr(regx, rega, regb, imm); break;
    case 0x04: sar(regx, rega, regb, imm); break;
    case 0x05: and(regx, rega, regb, imm); break;
    case 0x06: or(regx, rega, regb, imm);  break;
    case 0x07: xor(regx, rega, regb, imm); break;
    case 0x18: cmpne(regx, rega, regb, imm); break;
    case 0x19: cmpeq(regx, rega, regb, imm); break;
    case 0x1a: cmplt(regx, rega, regb, imm); break;
    case 0x1b: cmple(regx, rega, regb, imm); break;
    case 0x1c: fcmpne(regx, rega, regb);     break;
    case 0x1d: fcmpeq(regx, rega, regb);     break;
    case 0x1e: fcmplt(regx, rega, regb);     break;
    case 0x1f: fcmple(regx, rega, regb);     break;
    default:  error("invalid optag: %08x", tag);       break;
    } break;
    /* FPU */
  case 0x1:
    sign_mode = s;
    fpu_cnt[tag]++;
    switch (tag) {
    case 0x00: fadd(regx, rega, regb);       break;
    case 0x01: fsub(regx, rega, regb);       break;
    case 0x02: fmul_(regx, rega, regb);      break;
    case 0x03: fdiv(regx, rega, regb);       break;
    case 0x04: finv(regx, rega);             break;
    case 0x05: fsqrt(regx, rega);            break;
    case 0x06: ftoi(regx, rega);             break;
    case 0x07: itof(regx, rega);             break;
    case 0x08: floor_(regx, rega);           break;
    default:  error("invalid optag: %08x", tag);       break;
    } break; 
  case 0x2: ldl(rega, regb, disp); ldl_cnt++; break;
  case 0x3: ldh(rega, regb, disp); ldh_cnt++; break;
    /* SYS */
  case 0x4: puts("sysenter..."); break;
  case 0x5: puts("sysexit...");  break;
    /* MEMORY */
  case 0x6: st(rega, regb, disp);  st_cnt++;  break;
  case 0x8: ld(rega, regb, disp);  ld_cnt++;  break;
    /* BRANCH */
  case 0xb: jl(rega, disp); jl_cnt++; break;
  case 0xc: jr(rega);       jr_cnt++; break;
  case 0xd: bne(rega, regb, disp);  bne_cnt++;  break;
  case 0xf: beq(rega, regb, disp);  beq_cnt++;  break;
    /* Error */
  default:  error("invalid opcode: %08x", code); break;
  }
  
  error_check();
  
  if (ireg[STAC_PTR] < max_stack) max_stack = ireg[STAC_PTR];
}

void decode_opcode(uint32_t code, char *order)
{
  uint32_t opcode = 0, regx = 0, rega = 0, regb = 0, s = 0, tag = 0, pred = 0;
  int16_t imm = 0, disp = 0;
  int mode; /* 0..alu, 1..fpu, 2..mem */
  char *ord, *suffix;

  opcode = code >> 28;

  switch (opcode) {
  case 0x0: split_alu(code, &regx, &rega, &regb, &imm, &tag); mode = 0; break;
  case 0x1: split_fpu(code, &regx, &rega, &regb, &s, &tag);   mode = 1; break;
  default:  split_mem(code, &rega, &regb, &pred, &disp);      mode = 2; break;
  }

  switch (opcode) {
    /* ALU */
  case 0x0:
    switch (tag) {
    case 0x00: ord = "add";      break;
    case 0x01: ord = "sub";      break;
    case 0x02: ord = "shl";      break;
    case 0x03: ord = "shr";      break;
    case 0x04: ord = "sar";      break;
    case 0x05: ord = "and";      break;
    case 0x06: ord = "or";       break;
    case 0x07: ord = "xor";      break;
    case 0x18: ord = "cmpne";    break;
    case 0x19: ord = "cmpeq";    break;
    case 0x1a: ord = "cmplt";    break;
    case 0x1b: ord = "cmple";    break;
    case 0x1c: ord = "fcmpne";  break;
    case 0x1d: ord = "fcmpeq";  break;
    case 0x1e: ord = "fcmplt";  break;
    case 0x1f: ord = "fcmple";  break;
    default:  error("invalid optag: %08x", tag);       break;
    } break;
    /* FPU */
  case 0x1:
    switch (tag) {
    case 0x00: ord = "fadd";    break;
    case 0x01: ord = "fsub";    break;
    case 0x02: ord = "fmul";    break;
    case 0x03: ord = "fdiv";    break;
    case 0x04: ord = "finv";    break;
    case 0x05: ord = "fsqrt";   break;
    case 0x06: ord = "ftoi";    break;
    case 0x07: ord = "itof";    break;
    case 0x08: ord = "floor";   break;
    default:  error("invalid optag: %08x", tag);       break;
    } break; 
  case 0x2: ord = "ldl"; break;
  case 0x3: ord = "ldh"; break;
    /* SYS */
  case 0x4: ord = "sysenter"; break;
  case 0x5: ord = "sysexit";  break;
    /* MEMORY */
  case 0x6: ord = "st";  break;
  case 0x8: ord = "ld";  break;
    /* BRANCH */
  case 0xb: ord = "jl"; break;
  case 0xc: ord = "jr"; break;
  case 0xd: ord = "bne";  break;
  case 0xf: ord = "beq";  break;
    /* Error */
  default:  error("invalid opcode: %08x", code); break;
  }

  if (mode == 0) {
    sprintf(order, "%s r%d, r%d, r%d, %d", ord, regx, rega, regb, imm);
  } else if (mode == 1) {
    if      (s == 0) suffix = "";
    else if (s == 1) suffix = ".neg";
    else if (s == 2) suffix = ".abs";
    else if (s == 3) suffix = ".abs.neg";
    sprintf(order, "%s%s f%d, f%d, f%d, %d", ord, suffix, regx, rega, regb, imm);
  } else {
    if (pred == 0) suffix = "";
    else           suffix = "+";
    sprintf(order, "%s%s r%d, r%d, %d", ord, suffix, rega, regb, disp);
  }      
}

