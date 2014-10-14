#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "runsim.h"
#include "mem.h"

#define HALT  (0xc0000000 + (INST_POINTER << 16))

int main(void)
{
  FILE *fp;
  char c;
  int i, j = 0;
  uint32_t order;

  initialize_reg();

  if ((fp = fopen("in.dat", "r")) == NULL) {
    perror("fopen");
    exit(-1);
  }

  while(1){
    order = 0;

    //  for (i=0; i<4 && (c = fgetc(fp)) != EOF; i++) {
      /* issue: ffとEOFとの区別がつかない */
    for (i=0; i<4; i++) { // とりあえず応急処置
      c = fgetc(fp); // haltを読み込んだ時点で終わり
      order <<= 8;
      order += ((unsigned int)c & 0xff);
    }

    memory[j] = order;

    // if (c == EOF) break;

    if (order == HALT) break;

    j++;

  }

  fclose(fp);

  while(reg[INST_POINTER] < j){
    runsim(memory[reg[INST_POINTER]]);
    reg[INST_POINTER]++;

    if (memory[reg[INST_POINTER]] == HALT) break;

  }
  
  print_reg();

  return 0; 
}
