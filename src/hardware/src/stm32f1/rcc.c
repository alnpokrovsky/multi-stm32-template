#if defined(STM32F1)

#include "rcc.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/rcc.h>

uint32_t rcc_core_freq = 72000000;

void rcc_init() {
#if HSE_Value == 8000000
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
#elif HSE_Value == 16000000
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE16_72MHZ]);
#else
#error "HSE value not supported"
#endif
}


#endif