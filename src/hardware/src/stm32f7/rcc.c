#if defined(STM32F7)

#include "rcc.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/rcc.h>

uint32_t rcc_core_freq = 216000000;

void rcc_init() {
    rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_216MHZ], HSE_Value/1000000);
}


#endif