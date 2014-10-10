#include <stdio.h>
#include <stdint.h>

#include "runsim.h"

int main(void)
{
  uint32_t reg[16];
  int i;
  int16_t test;

  for (i=0; i<16; i++) {
    reg[i] = 0;
  }

  runsim(0x01000001, reg); // $1 <= 1
  runsim(0x02000002, reg); // $2 <= 2
  runsim(0x03120000, reg); // $3 <= $1 + $2 = 3
  runsim(0x14320000, reg); // $4 <= $3 - $2 = 1
  runsim(0x25300001, reg); // $5 <= $3 << 1 = 6
  runsim(0x2650ffff, reg); // $6 <= $5 >> 1 = 3
  runsim(0x37300000, reg); // $7 <= -$3
  runsim(0x0820fffd, reg); // $8 <= $2 + (-3) = -1

  for (i=0; i<16; i++) {
    printf("$%d = %x\n", i, reg[i]);
  }


  puts("");
  test = -3;
  printf("%x\n", test);

  return 0;
}
