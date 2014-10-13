#include <stdio.h>
#include <stdint.h>

#include "runsim.h"
#include "mem.h"


int main(void)
{


  initialize_reg();
  
  memory[0] =  0x01000001; // $1 <= 1
  memory[1] =  0x02000002; // $2 <= 2
  memory[2] =  0x03120000; // $3 <= $1 + $2 = 3
  memory[3] =  0x14320000; // $4 <= $3 - $2 = 1
  memory[4] =  0x25300001; // $5 <= $3 << 1 = 6
  memory[5] =  0x2650ffff; // $6 <= $5 >> 1 = 3
  memory[6] =  0x37300000; // $7 <= -$3
  memory[7] =  0x0820fffd; // $8 <= $2 + (-3) = -1
  memory[8] =  0x95000800; // Memory[0x800] <= $5
  memory[9] =  0x89000800; // $9 <= Memory[0x800]
  memory[10] = 0x0aa00001; // $10 <= $10 + 1
  memory[11] = 0xca90000d; // if ($10 == $9) goto memory[13]
  memory[12] = 0xd120000a; // if ($1 < $2) goto memory[10]
  memory[13] = 0x0b000abc; // $11 <= 0xabc 
  memory[14] = 0xac000000; // $12 <= stdin
  memory[15] = 0xbc000000; // stdout <= $12

  while(reg[INST_POINTER] < 16){
    runsim(memory[reg[INST_POINTER]]);
    reg[INST_POINTER]++;
  }

  print_reg();
  return 0;
}

