#include <stdio.h>
#include <stdint.h>

#include "exec_fpu.h"
#include "mem.h"

union Ui_f 
{
  uint32_t n;
  float f;
};

void fadd(uint32_t rd, uint32_t rs, uint32_t rt)
{
  union Ui_f ui_fd, ui_fs, ui_ft;

  ui_fs.n = reg[rs];
  ui_ft.n = reg[rt];

  ui_fd.f = ui_fs.f + ui_ft.f;

  if (ui_fd.n == 0x80000000) { // -0
    ui_fd.n = 0;
  }
  reg[rd] = ui_fd.n;
}

void fmul(uint32_t rd, uint32_t rs, uint32_t rt)
{
  union Ui_f ui_fd, ui_fs, ui_ft;

  ui_fs.n = reg[rs];
  ui_ft.n = reg[rt];

  ui_fd.f = ui_fs.f * ui_ft.f;

  if (ui_fd.n == 0x80000000) { // -0
    ui_fd.n = 0;
  }
  reg[rd] = ui_fd.n;
}
  
