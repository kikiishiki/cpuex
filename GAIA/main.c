#include "cnt.h"
#include "env.h"
#include "util.h"
#include "io.h"
#include "runsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define HALT  (0x88000000)

char infile[128];
int step_exec = 0;
int breakpoint[MEM_SIZE];
enum Add_Mode { A_DEC, A_HEX };
enum Add_Mode addr_mode = A_HEX;

int step_fun();


void parse_args(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    if (argv[i][0] != '-') {
      strcpy(infile, argv[i]);
    } else if (argv[i][1] == 'r') {
      switch (argv[i][2]) {
        case 'r': read_mode = RAW;   break;
        case 'i': read_mode = INT;   break;
        case 'f': read_mode = FLOAT; break;
        case 'x': read_mode = HEX;   break;
        default: fprintf(stderr, "invalid option: %s\n", argv[i]); exit(1);
      }
    } else if (argv[i][1] == 'w') {
      switch (argv[i][2]) {
        case 'r': write_mode = RAW;   break;
        case 'i': write_mode = INT;   break;
        case 'f': write_mode = FLOAT; break;
        case 'x': write_mode = HEX;   break;
        default: fprintf(stderr, "invalid option: %s\n", argv[i]); exit(1);
      }
    } else if (strcmp(argv[i], "-debug") == 0) {
      step_exec = 1;
      for (i = 0; i < (1 << 20); ++i) breakpoint[i] = 0;
    } else {
      fprintf(stderr, "invalid option: %s\n", argv[i]);
      exit(1);
    }
  }
}

void initialize() {
  initialize_cnt();
  read_pos = write_pos = 0;
  initialize_env();
}

void simulate() {
  initialize();
  for (;;) {
    if (mem[prog_cnt] == HALT) break;
    ++inst_cnt;
    if (breakpoint[prog_cnt] == 1)
      step_exec = 1;
    if (step_exec) while (step_fun());    runsim(mem[prog_cnt]);
    prog_cnt++;
  }
}


int main(int argc, char *argv[])
{
  FILE *fp;
  char c;
  int i, halt_cnt = 0;
  uint32_t order;

  strcpy(infile, "in.dat");
  parse_args(argc, argv);

  if ((fp = fopen(infile, "r")) == NULL) {
    perror("fopen");
    fprintf(stderr, "infile: %s\n", infile);
    exit(1);
  }

  for (prog_size = 1; ; ++prog_size) {
    order = 0;
    for (i = 0; i < 4; i++) {
      c = fgetc(fp); // halt*3を読み込んだ時点で終わり
      order <<= 8;
      order += ((unsigned int)c & 0xff);
    }
    mem[HEAP_ADDR + prog_size] = order;
    if (order == HALT) { if (++halt_cnt == 3) break; }
    else halt_cnt = 0;
  }

  fclose(fp);
  
  simulate();
  
  fprintf(stderr, "<executed %lld instructions.>\n", inst_cnt);
  print_env(stderr);
  print_cnt(stderr);

  return 0;
}

int read_address(char *addr);
float to_float(uint32_t i);

int step_fun()
{
  char c;
  char command[128], order[256];
  char *tok;
  int i;
  int addr, to_addr;
  int pm_mode = 0; /* pm_mode...0:default, 1:float, 2:opcode */
  
  decode_opcode(mem[prog_cnt], order);
  printf("%lld(0x%05x):\t%s(0x%08x)\n", inst_cnt, prog_cnt+1, order, mem[prog_cnt]);

  printf("sim > ");
  
  for (i=0; i<128 && (c = getchar()) != '\n'; i++) {
    command[i] = c;
  }

  command[i] = '\0';

  tok = strtok(command, " \t\n");

  if (!tok || !strcmp(tok, "s") || !strcmp(tok, "step")) {
    step_exec = 1;
    return 0;
  }
 
  else if (!strcmp(tok, "r") || !strcmp(tok, "run")) {
    step_exec = 0;
    return 0;
  }
 
  else if (!strcmp(tok, "b") || !strcmp(tok, "break")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Please enter the memory address.");
      return 1;
    }
    if ((addr = read_address(tok)) < 0) {
      return 1;
    }
    breakpoint[HEAP_ADDR + addr] = 1;
    printf("Breakpoint %d (0x%05x)\n", HEAP_ADDR + addr, addr);
    return 1;
  }
 
  else if (!strcmp(tok, "db") || !strcmp(tok, "delete_bp")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Please enter the memory address.");
      return 1;
    }
    if ((addr = read_address(tok)) < 0) {
      return 1;
    }
    breakpoint[HEAP_ADDR + addr] = 0;
    printf("Delete Breakpoint %d (0x%05x)\n", HEAP_ADDR + addr, addr);
    return 1;
  }
 
  else if (!strcmp(tok, "pe") || !strcmp(tok, "print_env")) {
    print_env(stdout);
    return 1;
  }
  
  else if (!strcmp(tok, "pm") || !strcmp(tok, "print_mem")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Util : pm <option> [addr]");
      puts("----------");
      puts("pm -f [addr]  : 浮動小数点数で出力");
      puts("pm -c [addr]  : opcodeと見なして、デコードして出力");
      return 1;
    } else if (!strcmp(tok, "-f")) {
      pm_mode = 1;
      tok = strtok(NULL, " \t\n"); 
    } else if (!strcmp(tok, "-c")) {
      pm_mode = 2;
      tok = strtok(NULL, " \t\n");
    }
    if ((addr = read_address(tok)) < 0) {
      return 1;
    }
    if (pm_mode == 0)
      printf("mem[0x%05x]: %11d (0x%08x)\n", addr+1, mem[addr], mem[addr]);
    else if (pm_mode == 1) 
      printf("mem[0x%05x]: %f\t(0x%08x)\n", addr+1, to_float(mem[addr]), mem[addr]);
    else {
      decode_opcode(mem[addr], order);
      printf("mem[0x%05x]: %s\t(0x%08x)\n", addr+1, order, mem[addr]);
    }
    return 1;
  }

  else if (!strcmp(tok, "pms") || !strcmp(tok, "print_mems")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Util : pms <option> [addr1] [addr2]");
      puts("----------");
      puts("pms -f ...    : 浮動小数点数で出力");
      puts("pms -c ...    : opcodeと見なして、デコードして出力");
      return 1;
    } else if (!strcmp(tok, "-f")) {
      pm_mode = 1;
      tok = strtok(NULL, " \t\n"); 
    } else if (!strcmp(tok, "-c")) {
      pm_mode = 2;
      tok = strtok(NULL, " \t\n");
    }
    if ((addr = read_address(tok)) < 0) {
      return 1;
    }
    tok = strtok(NULL, " \t\n");
    if ((to_addr = read_address(tok)) < 0) {
      return 1;
    }
    if (addr > to_addr) {
      puts("invalid address");
      return 1;
    }
    if (pm_mode == 0)
      for (; addr<=to_addr; addr++)
	printf("mem[0x%05x]: %11d (0x%08x)\n", addr+1, mem[addr], mem[addr]);
    else if (pm_mode == 1) 
      for (; addr<=to_addr; addr++)
	printf("mem[0x%05x]: %f\t(0x%08x)\n", addr+1, to_float(mem[addr]), mem[addr]);
    else {
      for (; addr<=to_addr; addr++) {
	decode_opcode(mem[addr], order);
	printf("mem[0x%05x]: %s\t(0x%08x)\n", addr+1, order, mem[addr]);
      }
    }
    return 1;
  }
  
  else if (!strcmp(tok, "pc") || !strcmp(tok, "print_cnt")) {
    print_cnt(stdout);
    return 1;
  }

  else if (!strcmp(tok, "re") || !strcmp(tok, "rerun")) {
    initialize_env();
    initialize_cnt();
    return 1;
  }

  else if (!strcmp(tok, "addr_mode")) {
    tok = strtok(NULL, " \t\n");
    if (!tok) {
      puts("Address input mode");
      puts("----------");
      puts("addr_mode -d  : 10進数で入力");
      puts("addr_mode -x  : 16進数で入力");
    } else if (!strcmp(tok, "-d")) {
      puts("Address input mode: DEC");
      addr_mode = A_DEC;
    } else if (!strcmp(tok, "-x")) {
      puts("Address input mode: HEX");
      addr_mode = A_HEX;
    } else {
      puts("invalid option.");
    }
    return 1;
  }
 
  else if (!strcmp(tok, "h") || !strcmp(tok, "help")) {
    puts("Commands");
    puts("----------");
    puts("r,         run             : プログラムを実行");
    puts("s,         step            : 1命令実行");
    puts("b [addr],  break [addr]    : memory[addr]にブレークポイントを設定");
    puts("db [addr], delete_bp [addr]: memory[addr]のブレークポイントを解除");
    puts("pe,        print_env       : 環境を表示");
    puts("pm [addr], print_mem [addr]: memory[addr]の内容を表示");
    puts("pms [addr1] [addr2]        : memory[addr1]からmemory[addr2]までの内容を表示");
    puts("pc,        print_cnt       : 各命令の実行回数を表示");
    puts("re,        rerun           : 命令をはじめから実行");
    puts("addr_mode                  : [addr]の入力形式を指定");
    puts("h,         help            : ヘルプを表示");

    return 1;

  } 
  else {
    puts("Undefined command. Please try \"h\" or \"help\".");
    return 1;
  }
}

int read_address(char *addr)
{
  int address = 0;
  int i = 0;

  if (addr_mode == A_DEC) {
    address = atoi(addr);
  } else if (addr_mode == A_HEX) {
    while (addr[i] != '\0') {
      address <<= 4;
      if ('0' <= addr[i] && addr[i] <= '9') {
	address += addr[i] - '0';
      } else if ('a' <= addr[i] && addr[i] <= 'f') {
	address += addr[i] - 'a' + 10;
      } else if ('A' <= addr[i] && addr[i] <= 'F') {
	address += addr[i] - 'A' + 10;
      } else {
	address >>= 4;
	break;
      }
      i++;
    }
  }

  if (address > MEM_SIZE) {
    puts("The memory address is too big!");
    address = -1;
  }

  return address - 1;
}

float to_float(uint32_t i)
{
  union Ui_f_ { uint32_t n; float f; } ui_f_;

  ui_f_.n = i;
  return ui_f_.f;
}
