#if defined(STM32F1)

#include "rcc.h"
#include <libopencm3/stm32/rcc.h>


void rcc_init() {
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}


#endif