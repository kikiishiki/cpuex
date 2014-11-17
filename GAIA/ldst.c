#include "env.h"
#include "cnt.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

uint32_t load(uint32_t address) 
{
  int i;
  uint32_t data;
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
