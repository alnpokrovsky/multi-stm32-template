#ifndef __HARDWARE_SDRAM_H__
#define __HARDWARE_SDRAM_H__

#include <stdint.h>

extern uint8_t * const _sdram;
extern uint8_t * const _esdram;

void sdram_init(void);

#endif
