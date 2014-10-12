#include <stdio.h>
#include <stdint.h>

#include "mem.h"


void initialize_reg()
{
  reg[0] = 0;
  reg[STAC_POINTER] = 0x00100000;
  reg[BASE_POINTER] = 0x00100000;
  reg[INST_POINTER] = 0;
}

void print_reg()
{
  int i;

  for (i=0; i<REG_NUM; i++){
    printf("$%d = %x\n", i, reg[i]);
  }
}
