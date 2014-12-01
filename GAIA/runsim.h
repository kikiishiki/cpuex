#ifndef RUNSIM_H_
#define RUNSIM_H_

#include <stdint.h>

void runsim(uint32_t code);
void decode_opcode(uint32_t code, char *order);

#endif
