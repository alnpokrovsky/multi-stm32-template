#if defined(STM32F1)

#include "rcc.h"
#include <libopencm3/stm32/rcc.h>


void rcc_init() {
	rcc_clock_setup_in_hse_8mhz_out_24mhz();
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_DIV2);
}


#endif