#if defined(STM32F1)

#include "rcc.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/rcc.h>

uint32_t rcc_core_freq = 72000000;

void rcc_init() {
#if HSE_Value == 8000000
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	//rcc_clock_setup_in_hse_8mhz_out_24mhz();
#elif HSE_Value == 16000000
	rcc_clock_setup_in_hse_16mhz_out_72mhz();
#else
#error "HSE value not supported"
#endif
}


#endif