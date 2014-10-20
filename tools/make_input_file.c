#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  FILE *fin, *fout;
  char c;
  int i=0, temp=0;

  if ((fin = fopen("in.txt", "r")) == NULL) {
    perror("fopen");
    exit(-1);
  }

  if ((fout = fopen("in.dat", "w")) == NULL) {
    perror("fopen");
    exit(-1);
  }

  while ((c = fgetc(fin)) != EOF) {
    if ('0' <= c && c <= '9') {
      temp += c - '0';
    } else if ('a' <= c && c <= 'f') {
      temp += c - 'a' + 10;
    } else if ('A' <= c && c <= 'F') {
      temp += c - 'A' + 10;
    } else if (c == '\n') {
      break;
    } else {
      printf("invalid input : %c\n", c);
      exit(-1);
    }

    if (i == 0) {
      temp <<= 4;
      i = 1;
    } else {
      fputc((char)temp, fout);
      temp = 0;
      i = 0;
    }

  }

  fclose(fin);
  fclose(fout);

  return 0;
}
