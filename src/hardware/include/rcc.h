#ifndef __HARDWARE_RCC_H__
#define __HARDWARE_RCC_H__

#include <stdint.h>

extern uint32_t rcc_core_freq;

/* init hse quarts clocks */
void rcc_init(void);

#endif
