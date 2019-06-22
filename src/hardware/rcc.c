#include "rcc.h"
#include <libopencm3/stm32/rcc.h>


void rcc_init() {
#if defined(STM32F1)
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
#elif defined(STM32F3)
	rcc_clock_setup_pll(&rcc_hse8mhz_configs[RCC_CLOCK_HSE8_72MHZ]);
#elif defined(STM32F4)
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
#endif
}