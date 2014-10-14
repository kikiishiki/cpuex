#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "runsim.h"
#include "mem.h"
#include "flags.h"

#define HALT  (0xc0000000 + (INST_POINTER << 16))

extern int step_fun();

int main(int argc, char *argv[])
{
  FILE *fp;
  char c;
  int i, j = 0;
  uint32_t order;

  if (argc > 1) {
    if (!strcmp(argv[1], "-debug")) {
      step_exec = 1;
      for (i=0; i< (1<<20); i++) {
	breakpoint[i] = 0;
      }
    }
  }

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

    if (breakpoint[reg[INST_POINTER]] == 1) {
      step_exec = 1;
    }
    if (step_exec) {
      while (step_fun());
    }

    runsim(memory[reg[INST_POINTER]]);
    reg[INST_POINTER]++;

    if (memory[reg[INST_POINTER]] == HALT) break;

  }
  
  print_reg();

  return 0; 
}

int step_fun()
{
  char c;
  char command[128];
  char *tok;
  int i;
  int bp;
  
  printf("%d:\t0x%x\n", reg[INST_POINTER], memory[reg[INST_POINTER]]);

  printf("sim > ");
  
  for (i=0; i<128 && (c = getchar()) != '\n'; i++) {
    command[i] = c;
  }

  command[i] = '\0';

  tok = strtok(command, " \t\n");

  if (!tok || !strcmp(tok, "s") || !strcmp(tok, "step")) {
    step_exec = 1;
    return 0;
  } else if (!strcmp(tok, "r") || !strcmp(tok, "run")) {
    step_exec = 0;
    return 0;
  } else if (!strcmp(tok, "b") || !strcmp(tok, "break")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Please enter the memory address.");
      return 1;
    }
    bp = atoi(tok);
    breakpoint[bp] = 1;
    printf("Breakpoint %d\n", bp);
    return 1;
  } else if (!strcmp(tok, "db") || !strcmp(tok, "delete_bp")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Please enter the memory address.");
      return 1;
    }
    bp = atoi(tok);
    breakpoint[bp] = 0;
    printf("Delete Breakpoint %d\n", bp);
    return 1;
  } else if (!strcmp(tok, "pr") || !strcmp(tok, "print_reg")) {
    print_reg();
    return 1;
  } else if (!strcmp(tok, "h") || !strcmp(tok, "help")) {
    printf("Commands\n----------\nr,        run            : プログラムを実行\ns,        step           : 1命令実行\nb [add],  break [add]    : memory[add]にブレークポイントを設定\ndb [add], delete_bp [add]: memory[add]のブレークポイントを解除\npr,       print_reg      : レジスタの内容を表示\nh,        help           : ヘルプを表示\n");

    return 1;

  } else {
    puts("Undefined command. Please try \"h\" or \"help\".");
    return 1;
  }
}


