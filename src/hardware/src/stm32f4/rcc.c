#if defined(STM32F4)

#include "rcc.h"
#include <libopencm3/stm32/rcc.h>
#include "DeviceConfig.h"

uint32_t rcc_core_freq = 168000000;

void rcc_init() {
#if HSE_Value == 8000000
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
#else
#error "HSE value not supported"
#endif
}


#endif