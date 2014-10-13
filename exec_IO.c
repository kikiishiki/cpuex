#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "exec_IO.h"
#include "mem.h"

#define X_MODE 0  // 16進数
#define B_MODE 1  // 2進数

#define READ_MODE X_MODE
#define READ_NUM ((READ_MODE == X_MODE ? 2 : 8))

#define WRITE_MODE X_MODE
#define WRITE_NUM ((WRITE_MODE == X_MODE ? 2 : 8))

void io_read(uint32_t rd)
{
  /* 標準入力から1byteをread */

  char buf[READ_NUM+1];
  char c;
  int i = 0;
  uint32_t input = 0;
  FILE* fin = stdin;
  
  while((c = fgetc(fin)) != EOF && i<READ_NUM){
    buf[i++] = c;
  }
  buf[READ_NUM] = '\0';

  if (READ_MODE == X_MODE) {
    for (i=0; i<READ_NUM; i++) {

      input <<= 4;

      if ('0' <= buf[i] && buf[i] <= '9') {
	input += buf[i] - '0';
      } else if ('a' <= buf[i] && buf[i] <= 'f') {
	input += buf[i] - 'a' + 10;
      } else if ('A' <= buf[i] && buf[i] <= 'F') {
	input += buf[i] - 'A' + 10;
      } else {
	puts("invalid input");
	exit(-1);
      }
    }

      reg[rd] = input;
    
  } else if (READ_MODE == B_MODE) {
    for (i=0; i<READ_NUM; i++) {

      input <<= 1;

      if (buf[i] == '1') {
	input += 1;
      } else if (buf[i] != '0') {
	puts("invalid input");
	exit(-1);
      }
      
    }

    reg[rd] = input;
    
  }
}


void io_write(uint32_t rd)
{
  /* 標準出力へ1byteをwrite */
  char buf[WRITE_NUM+1];
  int i;
  uint32_t output, temp;
  FILE* fout = stdout;

  output = reg[rd];

  if (WRITE_MODE == X_MODE) {
    for (i=0; i<WRITE_NUM; i++) {

      temp = output & 0xf;

      if (temp <= 0x9) {
	buf[WRITE_NUM - i - 1] = '0' + temp;
      } else {
	buf[WRITE_NUM - i - 1] = 'a' + temp - 10;
      }

      output >>= 4;

    }
    
  } else if (READ_MODE == B_MODE) {
    for (i=0; i<WRITE_NUM; i++) {

      temp = output & 0x1;

      if (temp == 0x0) {
	buf[WRITE_NUM - i - 1] = '0';
      } else {
	buf[WRITE_NUM - i - 1] = '1';
      }

      output >>= 1;

    }
    
  }
   
  buf[WRITE_NUM] = '\0';

  fputs(buf, fout);
  
}
