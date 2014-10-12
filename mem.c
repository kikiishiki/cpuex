#include <stdio.h>
#include <stdint.h>

#include "mem.h"

#define INST_POINTER  15
#define BASE_POINTER
#define STAC_POINTER

void intialize_reg()
{
  reg[0] = 0;
  reg[INST_POINTER] = 0;
}

void print_reg()
{
  int i;

  for (i=0; i<REG_NUM; i++){
    printf("$%d = %x\n", i, reg[i]);
  }
}
