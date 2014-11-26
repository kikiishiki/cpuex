#include "cnt.h"
#include "env.h"
#include "util.h"
#include "ldst.h"
#include "io.h"
#include "runsim.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern uint32_t fmul();
extern uint32_t load();
extern void store();
union Ui_f { uint32_t n; float f; };

void add(uint32_t rx, uint32_t ra, uint32_t rb, uint16_t imm) {
  /* オーバーフローはとりあえず無視 */
  ireg[rx] = ireg[ra] + ireg[rb] + imm;
}

void sub(uint32_t rx, uint32_t ra, uint32_t rb, uint16_t imm) {
  ireg[rx] = ireg[ra] - ireg[rb] - imm;
}

void shl(uint32_t rx, uint32_t ra, uint32_t rb, uint16_t imm) {
  ireg[rx] = ireg[ra] << (ireg[rb] + imm);
}

void shr(uint32_t rx, uint32_t ra, uint32_t rb, uint16_t imm) {
  ireg[rx] = ireg[ra] >> (ireg[rb] + imm);
}

void sar(uint32_t rx, uint32_t ra, uint32_t rb, uint16_t imm) {
  ireg[rx] = (ireg[ra] >> (ireg[rb] + imm)) | (0xffffffff << (32 - ireg[rb] - imm));
}

void and(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] & ireg[rb];
}

void or(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] | ireg[rb];
}

void not(uint32_t rx, uint32_t ra) {
  ireg[rx] = ~(ireg[ra]);
}

void xor(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] ^ ireg[rb];
}

void cat(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = (ireg[ra] & 0xffff0000) | (ireg[rb] & 0x0000ffff);
}

void cmpne(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] != ireg[rb] ? -1 : 0;
}

void cmpeq(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] == ireg[rb] ? -1 : 0;
}

void cmplt(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] < ireg[rb] ? -1 : 0;
}

void cmple(uint32_t rx, uint32_t ra, uint32_t rb) {
  ireg[rx] = ireg[ra] <= ireg[rb] ? -1 : 0;
}

void ldl(uint32_t rx, uint32_t ra, uint16_t imm) {
  ireg[rx] = (ireg[ra] & 0xffff0000) | ((uint32_t)imm & 0x0000ffff);
}

void ldh(uint32_t rx, uint32_t ra, uint16_t imm) {
  ireg[rx] = (((uint32_t)imm << 16) & 0xffff0000) | (ireg[ra] & 0x0000ffff);
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

void ffma(uint32_t fx, uint32_t fa, uint32_t fb, uint32_t fc) {
}

void fcat(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = operate_sign_bit( (freg[fa] & 0xffff0000) | (freg[fb] & 0x0000ffff) );
}

void fand(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = operate_sign_bit( freg[fa] & ireg[fb] );
}

void f_or(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = operate_sign_bit( freg[fa] | freg[fb] );
}

void fxor(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = operate_sign_bit( freg[fa] ^ freg[fb] );
}

void fnot(uint32_t fx, uint32_t fa) {
  freg[fx] = operate_sign_bit( ~(freg[fa]) );
}

void fcmpne(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = freg[fa] != freg[fb] ? -1 : 0;
}

void fcmpeq(uint32_t fx, uint32_t fa, uint32_t fb) {
  freg[fx] = freg[fa] == freg[fb] ? -1 : 0;
}

void fcmplt(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  freg[fx] = ui_fa.f < ui_fb.f ? -1 : 0;
}

void fcmple(uint32_t fx, uint32_t fa, uint32_t fb) {
  union Ui_f ui_fa, ui_fb;
  ui_fa.n = freg[fa];
  ui_fb.n = freg[fb];
  freg[fx] = ui_fa.f <= ui_fb.f ? -1 : 0;
}

void fldl(uint32_t fx, uint32_t fa, uint16_t imm) {
  freg[fx] = (freg[fa] & 0xffff0000) | ((uint32_t)imm & 0x0000ffff);
}

void fldh(uint32_t fx, uint32_t fa, uint16_t imm) {
  freg[fx] = (((uint32_t)imm << 16) & 0xffff0000) | (freg[fa] & 0x0000ffff);
}

void ld(uint32_t rx, uint32_t ra, int16_t imm) {
  int32_t address;
  address = ireg[ra] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("load: invalid address: %x\n", address);
  ireg[rx] = load(address);
}

void st(uint32_t ra, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("store: invalid address: %x\n", address);
  store(address,ireg[ra]);
}

void fld(uint32_t fx, uint32_t ra, int16_t imm) {
  int32_t address;
  address = ireg[ra] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("load: invalid address: %x\n", address);
  freg[fx] = load(address);
}

void fst(uint32_t fa, uint32_t rb, int16_t imm) {
  int32_t address;
  address = ireg[rb] + imm;
  if (address < 0 || address >= MEM_SIZE)
    error("store: invalid address: %x\n", address);
  store(address,freg[fa]);
}

void bne(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("bne: invalid address: %x\n", address);
  if (ireg[ra] != ireg[rb])
    prog_cnt = address - 1;
}

void beq(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("beq: invalid address: %x\n", address);
  if (ireg[ra] == ireg[rb])
    prog_cnt = address - 1;
}

void blt(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("blt: invalid address: %x\n", address);
  if (ireg[ra] < ireg[rb])
    prog_cnt = address - 1;
}

void ble(uint32_t ra, uint32_t rb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("ble: invalid address: %x\n", address);
  if (ireg[ra] <= ireg[rb])
    prog_cnt = address - 1;
}

void fbne(uint32_t fa, uint32_t fb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("fbne: invalid address: %x\n", address);
  if (ireg[fa] != ireg[fb])
    prog_cnt = address - 1;
}

void fbeq(uint32_t fa, uint32_t fb, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("beq: invalid address: %x\n", address);
  if (ireg[fa] == ireg[fb])
    prog_cnt = address - 1;
}

void fblt(uint32_t fa, uint32_t fb, int16_t disp) {
}

void fble(uint32_t fa, uint32_t fb, int16_t disp) {
}

void jl(uint32_t rx, int16_t disp) {
  int32_t address;
  address = prog_cnt + disp;
  if (address < 0 || address >= MEM_SIZE)
    error("jl: invalid address: %x\n", address);
  ireg[rx] = prog_cnt + 1;
  prog_cnt = address - 1;
}

void jr(uint32_t ra) {
  prog_cnt = ireg[ra] - 1;
}

/* ここからrunsimの本体部分 */
void split_alu(uint32_t code, uint32_t opcode, uint32_t *regx, uint32_t *rega, uint32_t *regb, int16_t *imm, uint32_t *tag) {
  *regx = (code >> 22) & 0x1f;
  *rega = (code >> 17) & 0x1f;
  *regb = (code >> 12) & 0x1f;
  *imm  = (code >> 4)  & 0xff;
  if (opcode == 0)
    *imm &= 0x00ff; // 符号拡張なし
  else if ((*imm >> 7) == 1)
    *imm |= 0xff00; // 符号拡張あり
  *tag  = code & 0xf;
}

void split_fpu(uint32_t code, uint32_t *regx, uint32_t *rega, uint32_t *regb, uint32_t *regc, uint32_t *s, uint32_t *tag) {
  *regx = (code >> 22) & 0x1f;
  *rega = (code >> 17) & 0x1f;
  *regb = (code >> 12) & 0x1f;
  *regc = (code >> 7)  & 0x1f;
  *s    = (code >> 4)  & 0x7;
  *tag  = code & 0xf;
}

void split_mem(uint32_t code, uint32_t *rega, uint32_t *regb, uint32_t *pred, uint32_t *disp) { 
  *rega = (code >> 22) & 0x1f;
  *regb = (code >> 17) & 0x1f;
  *pred  = (code >> 16) & 0x1;
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
  uint32_t opcode = 0, regx = 0, rega = 0, regb = 0, regc = 0, s = 0, tag = 0, pred = 0, disp = 0;
  int16_t imm = 0;

  opcode = code >> 27;

  if (opcode == 0x2 || opcode == 0x3 || opcode == 0x6 || opcode == 0x7)
    split_mem(code, &rega, &regb, &pred, &disp);
  else {
    switch (opcode >> 2) {
    case 0x0: split_alu(code, opcode, &regx, &rega, &regb, &imm, &tag); break;
    case 0x1: split_fpu(code, &regx, &rega, &regb, &regc, &s, &tag); break;
    default:  split_mem(code, &rega, &regb, &pred, &disp); break;
    }
  }

  switch (opcode) {
    /* ALU */
  case 0x00:
  case 0x01:
    switch (tag) {
    case 0x0: add(regx, rega, regb, imm); add_cnt++;   break;
    case 0x1: sub(regx, rega, regb, imm); sub_cnt++;   break;
    case 0x2: shl(regx, rega, regb, imm); shl_cnt++;   break;
    case 0x3: shr(regx, rega, regb, imm); shr_cnt++;   break;
    case 0x4: sar(regx, rega, regb, imm); sar_cnt++;   break;
    case 0x5: and(regx, rega, regb);      and_cnt++;   break;
    case 0x6: or(regx, rega, regb);       or_cnt++;    break;
    case 0x7: not(regx, rega);            not_cnt++;   break;
    case 0x8: xor(regx, rega, regb);      xor_cnt++;   break;
    case 0x9: cat(regx, rega, regb);      cat_cnt++;   break;
    case 0xc: cmpne(regx, rega, regb);    cmpne_cnt++; break;
    case 0xd: cmpeq(regx, rega, regb);    cmpeq_cnt++; break;
    case 0xe: cmplt(regx, rega, regb);    cmplt_cnt++; break;
    case 0xf: cmple(regx, rega, regb);    cmple_cnt++; break;
    default:  error("invalid optag: %08x", tag);       break;
    } break;
  case 0x02: ldl(rega, regb, disp); ldl_cnt++; break;
  case 0x03: ldh(rega, regb, disp); ldh_cnt++; break;
    /* FPU */
  case 0x04:
    sign_mode = s;
    switch (tag) {
    case 0x00: fadd(regx, rega, regb);   fadd_cnt++;  break;
    case 0x01: fsub(regx, rega, regb);   fsub_cnt++;  break;
    case 0x02: fmul_(regx, rega, regb);  fmul_cnt++;  break;
    case 0x03: fdiv(regx, rega, regb);   fdiv_cnt++;  break;
    case 0x04: finv(regx, rega);         finv_cnt++;  break;
    case 0x05: fsqrt(regx, rega);        fsqrt_cnt++; break;
    case 0x06: ftoi(regx, rega);         ftoi_cnt++;  break;
    case 0x07: itof(regx, rega);         itof_cnt++;  break;
    case 0x08: floor_(regx, rega);       floor_cnt++; break;
    case 0x09: ffma(regx, rega, regb, regc); ffma_cnt++; break;
    case 0x0a: fcat(regx, rega, regb);   fcat_cnt++;  break;
    case 0x0b: fand(regx, rega, regb);   fand_cnt++;  break;
    case 0x0c: f_or(regx, rega, regb);   for_cnt++;   break;
    case 0x0d: fxor(regx, rega, regb);   fxor_cnt++;  break;
    case 0x0e: fnot(regx, rega);         fnot_cnt++;  break;
    case 0x1c: fcmpne(regx, rega, regb); fcmpne_cnt++; break;
    case 0x1d: fcmpeq(regx, rega, regb); fcmpeq_cnt++; break;
    case 0x1e: fcmplt(regx, rega, regb); fcmplt_cnt++; break;
    case 0x1f: fcmple(regx, rega, regb); fcmple_cnt++; break;
    default:  error("invalid optag: %08x", tag);       break;
    } break; 
  case 0x06: fldl(rega, regb, disp); fldl_cnt++; break;
  case 0x07: fldh(rega, regb, disp); fldh_cnt++; break;
    /* MEMORY */
  case 0x08: ld(rega, regb, disp);  ld_cnt++;  break;
  case 0x09: st(rega, regb, disp);  st_cnt++;  break;
  case 0x0a: fld(rega, regb, disp); fld_cnt++; break;
  case 0x0b: fst(rega, regb, disp); fst_cnt++; break;
    /* BRANCH */
  case 0x10: bne(rega, regb, disp);  bne_cnt++;  break;
  case 0x11: beq(rega, regb, disp);  beq_cnt++;  break;
  case 0x12: blt(rega, regb, disp);  blt_cnt++;  break;
  case 0x13: ble(rega, regb, disp);  ble_cnt++;  break;
  case 0x14: fbne(rega, regb, disp); fbne_cnt++; break;
  case 0x15: fbeq(rega, regb, disp); fbeq_cnt++; break;
  case 0x16: fblt(rega, regb, disp); fblt_cnt++; break;
  case 0x17: fble(rega, regb, disp); fble_cnt++; break;
    /* JUMP */
  case 0x18: jl(rega, disp); jl_cnt++; break;
  case 0x19: jr(rega);       jr_cnt++; break;
    /* Error */
  default:  error("invalid opcode: %08x", code); break;
  }
  
  error_check();
  
  if (ireg[STAC_PTR] < max_stack) max_stack = ireg[STAC_PTR];
}

