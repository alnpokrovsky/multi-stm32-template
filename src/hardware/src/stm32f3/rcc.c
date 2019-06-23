#if defined(STM32F3)

#include "rcc.h"
#include <libopencm3/stm32/rcc.h>


void rcc_init() {
	rcc_clock_setup_pll(&rcc_hse8mhz_configs[RCC_CLOCK_HSE8_72MHZ]);
}


#endif