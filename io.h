#ifndef IO_H_
#define IO_H_

#include <stdint.h>

#define BUF_SIZE 256

enum Mode { RAW, INT, FLOAT, HEX, INTERNAL };

extern enum Mode read_mode;
extern enum Mode write_mode;

extern char read_buf[BUF_SIZE];
extern char write_buf[BUF_SIZE];

extern int read_pos;
extern int write_pos;

void io_read(uint32_t rd);
void io_write(uint32_t rs);

#endif
