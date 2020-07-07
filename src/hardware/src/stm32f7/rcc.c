#if defined(STM32F7)

#include "rcc.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/rcc.h>

uint32_t rcc_core_freq = 216000000;

void rcc_init() {
	const struct rcc_clock_scale rcc = {
		.plln = 400,
		.pllp = 2,
		.pllq = 8,
		.hpre = RCC_CFGR_HPRE_DIV_NONE,
		.ppre1 = RCC_CFGR_PPRE_DIV_4,
		.ppre2 = RCC_CFGR_PPRE_DIV_2,
		.vos_scale = PWR_SCALE1,
		.overdrive = 1,
		.flash_waitstates = 6,
		.ahb_frequency = 200000000,
		.apb1_frequency = 50000000,
		.apb2_frequency = 100000000,
	};
	rcc_clock_setup_hse(&rcc, HSE_Value/1000000);
	// rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_216MHZ], HSE_Value/1000000);
    
	/* configure rcc pll clocking */
	RCC_PLLSAICFGR &= ~(RCC_PLLSAICFGR_PLLSAIN_MASK << RCC_PLLSAICFGR_PLLSAIN_SHIFT);
	RCC_PLLSAICFGR &= ~(RCC_PLLSAICFGR_PLLSAIQ_MASK << RCC_PLLSAICFGR_PLLSAIQ_SHIFT);
	RCC_PLLSAICFGR &= ~(RCC_PLLSAICFGR_PLLSAIR_MASK << RCC_PLLSAICFGR_PLLSAIR_SHIFT);
	RCC_PLLSAICFGR |= 192 << RCC_PLLSAICFGR_PLLSAIN_SHIFT;
	RCC_PLLSAICFGR |= 2   << RCC_PLLSAICFGR_PLLSAIQ_SHIFT;
	RCC_PLLSAICFGR |= 2   << RCC_PLLSAICFGR_PLLSAIR_SHIFT;
	RCC_DCKCFGR1 &= ~(RCC_DCKCFGR1_PLLSAIDIVR_MASK << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT);
	RCC_DCKCFGR1 |= RCC_DCKCFGR1_PLLSAIDIVR_DIVR_2 << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT;
	/* wait till RCC configured */
	RCC_CR |= RCC_CR_PLLSAION;
	while ((RCC_CR & RCC_CR_PLLSAIRDY) == 0) ;
}


#endif