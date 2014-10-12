#include <stdio.h>
#include <stdint.h>

#include "runsim.h"
#include "mem.h"


int main(void)
{
  int i;
  uint32_t orders[5];

  initialize_reg();

  for (i=0; i<3; i++) {
    orders[i] = 0x00000000;
  }

  orders[0] = 0x01000001; // $1 <= 1
  orders[1] = 0x02000002; // $2 <= 2
  orders[2] = 0x03120000; // $3 <= $1 + $2 = 3
  //  runsim(0x14320000); // $4 <= $3 - $2 = 1
  // runsim(0x25300001); // $5 <= $3 << 1 = 6
  //runsim(0x2650ffff); // $6 <= $5 >> 1 = 3
  //runsim(0x37300000); // $7 <= -$3
  //runsim(0x0820fffd); // $8 <= $2 + (-3) = -1
  //runsim(0x95000800); // Memory[0x800] <= $5
  //runsim(0x89000800); // $9 <= Memory[0x800]

  for (i=0; i<3; i++) {
    runsim(orders[i]);
  }

  print_reg();
  return 0;
}

