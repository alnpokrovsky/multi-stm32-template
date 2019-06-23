#if defined(STM32F4)

#include "rcc.h"
#include <libopencm3/stm32/rcc.h>


void rcc_init() {
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}


#endif