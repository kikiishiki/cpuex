#include "env.h"
#include "util.h"
#include "io.h"
#include <stdio.h>
#include <stdint.h>

enum Mode read_mode  = HEX;
enum Mode write_mode = HEX;

char read_buf[BUF_SIZE];
char write_buf[BUF_SIZE];

int read_pos  = 0;
int write_pos = 0;

union Ui_f { uint32_t n; float f; };


uint32_t io_read()
{
  char c;
  int d, i;
  union Ui_f ui;
  uint32_t ret = 0;

  switch (read_mode) {
    case RAW:
      if (scanf("%c", &c) <= 0)
        error("unexpected EOF");
      ret = c & 0xff;
      break;
    case INT:
      if (read_pos == 0) {
        if (scanf("%d", &d) <= 0)
          error("unexpected EOF");
        for (i = 3; i >= 0; --i) {
          read_buf[i] = d & 0xff;
          d >>= 8;
        }
      }
      ret = read_buf[read_pos++] & 0xff;
      if (read_pos == 4) read_pos = 0;
      break;
    case FLOAT:
      if (read_pos == 0) {
        if (scanf("%f", &ui.f) <= 0)
          error("unexpected EOF");
        for (i = 3; i >= 0; --i) {
          read_buf[i] = ui.n & 0xff;
          ui.n >>= 8;
        }
      }
      ret = read_buf[read_pos++] & 0xff;
      if (read_pos == 4) read_pos = 0;
      break;
    case HEX:
      if (scanf("%x", &d) <= 0)
        error("unexpected EOF");
      if (d < 0 || 0xff < d)
        error("invalid input: %02x", d);
      ret = d & 0xff;
      break;
    case INTERNAL:
      if (read_pos >= BUF_SIZE - 1)
        error("unexpected EOF");    
      ret = read_buf[read_pos++] & 0xff;
      break;
    default:
      error("internal error");
      break;
  }
  return ret;
}


void io_write(uint32_t data)
{
  int d = 0;
  int i;
  union Ui_f ui;

  switch (write_mode) {
    case RAW:
      putchar(data & 0xff);
      break;
    case INT:
      write_buf[write_pos++] = data & 0xff;
      if (write_pos == 4) {
        for (i = 0; i < 4; ++i) {
          d <<= 8;
          d += write_buf[i] & 0xff;
        }
        printf("%d\n", d);
        write_pos = 0;
      }
      break;
    case FLOAT:
      write_buf[write_pos++] = data & 0xff;
      if (write_pos == 4) {
        ui.n = 0;
        for (i = 0; i < 4; ++i) {
          ui.n <<= 8;
          ui.n += write_buf[i] & 0xff;
        }
        printf("%.15g\n", ui.f);
        write_pos = 0;
      }
      break;
    case HEX:
      printf("%02x\n", data & 0xff);
      break;
    case INTERNAL:
      write_buf[write_pos++] = data & 0xff;
      break;
    default:
      error("internal error");
      break;
  }
}

