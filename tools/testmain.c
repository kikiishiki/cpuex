#include <stdio.h>
#include <stdint.h>

#include "runsim.h"
#include "mem.h"


int main(void)
{


  initialize_reg();

  memory[0x1000] = 0x40400000; // 3.0
  memory[0x1001] = 0x3f000000; // 0.5
  
  memory[0] =  0x01000001; // $1 <= 1
  memory[1] =  0x02000002; // $2 <= 2
  memory[2] =  0x03120000; // $3 <= $1 + $2 = 3
  memory[3] =  0x14320000; // $4 <= $3 - $2 = 1
  memory[4] =  0x25300001; // $5 <= $3 << 1 = 6
  memory[5] =  0x2650ffff; // $6 <= $5 >> 1 = 3
  memory[6] =  0x87001000; // $7 <= Memory[0x1000]
  memory[7] =  0x88001001; // $8 <= Memory[0x1001]
  memory[8] =  0x95000800; // Memory[0x800] <= $5
  memory[9] =  0x49780000; // $9 <= $7 +. $8 = 3.5(0x40600000)
  memory[10] = 0x0aa00001; // $10 <= $10 + 1
  memory[11] = 0xca50000d; // if ($10 == $5) goto memory[13]
  memory[12] = 0xd120000a; // if ($1 < $2) goto memory[10]
  memory[13] = 0x0b000abc; // $11 <= 0xabc 
  memory[14] = 0xac000000; // $12 <= stdin
  memory[15] = 0xbc000000; // stdout <= $12 */

  /*
  memory[0] =  0x81001000; $1 <= 3.0(0x40400000)
  memory[1] =  0x82001001; $2 <= 0.5(0x3f000000)
  memory[2] =  0x43120000; $3 <= $1 + $2 = 3.5(0x40600000)
  memory[3] =  0x54120000; $4 <= $1 * $2 = 1.5(0x3fc00000)
  memory[4] =  0x35100000; $5 <= -$1 = -3.0(0xc0400000)
  */

  while(reg[INST_POINTER] < 16){
    runsim(memory[reg[INST_POINTER]]);
    reg[INST_POINTER]++;
  }

  print_reg();

  puts("");
  printf("%x\n", (uint8_t)EOF);

  return 0;
}

