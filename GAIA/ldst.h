#ifndef _LDST_H
#define _LDST_H

#include <stdint.h>

uint32_t load(uint32_t address);
void store(uint32_t address, uint32_t data);

#endif
