#include "env.h"
#include "io.h"
#include "cnt.h"
#include "util.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

uint32_t io_hundler(uint32_t opcode, uint32_t address, uint32_t data) {
  uint32_t ret = 0;

  if (opcode == 0x8) {
    if (address == 0x3000)
      ret = (read_pos >= BUF_SIZE ? 0 : 1);
    else if (address == 0x3004)
      ret = io_read();
    
    else if (address == 0x3008)
      ret = (write_pos >= BUF_SIZE ? 0 : 1);
    else 
      error("invalid address");
  }
  else if (opcode == 0x6) {
    if (address == 0x300c) {
      io_write(data);
      ret = 0;
    } 
    else {
      error("invalid address");
    }
  }
  return ret;
}

uint32_t load(uint32_t address) 
{
  int i;
  uint32_t data;

  if (0x3000 <= address && address < 0x4000) // memory mapped IO
    return io_hundler(0x8, address, 0);

  for (i = 0; i < CACHE_SIZE; i++) {
    if (cache[i] >> 32 == address) {
      data = (uint32_t)(cache[i] & 0x00000000ffffffff);
      break;
    }
  }
  if (i == CACHE_SIZE) { // data doesn't exist
    data = mem[address];
    miss_cnt++;
  }
  return data;
}

void store(uint32_t address, uint32_t data)
{
  uint64_t input;
  int i;

  if (0x3000 <= address && address < 0x4000) // memory mapped IO
    io_hundler(0x6, address, 0);

  srand((unsigned)time(NULL));
  input = (((uint64_t)address) << 32) | (uint64_t)data;
  for (i = 0; i < CACHE_SIZE; i++) {
    if (cache[i] >> 32 == address) {
      cache[i] = input;
      break;
    }
  }
  if (i == CACHE_SIZE) { // data doesn't exist
    cache[rand() % CACHE_SIZE] = input;
  }
  mem[address] = data;
}
