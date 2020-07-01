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
	// rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_168MHZ], HSE_Value/1000000);
    // // Enable HSE
    // RCC_CR |= RCC_CR_HSEON;
	// while (!(RCC_CR & RCC_CR_HSERDY));

	// FLASH_ACR |= FLASH_ACR_LATENCY_7WS;

	// RCC_PLLCFGR = 0x00;
    // // Div for HSE = 8
	// RCC_PLLCFGR |= 8 << RCC_PLLCFGR_PLLM_SHIFT;
    // // PLL mult x432
	// RCC_PLLCFGR |= 432 << RCC_PLLCFGR_PLLN_SHIFT;
    // // Source HSE
	// RCC_PLLCFGR |= RCC_PLLCFGR_PLLSRC;

	// RCC_CR |= RCC_CR_PLLON;
	// while((RCC_CR & RCC_CR_PLLRDY) == 0){}

	// RCC_CFGR &= ~(RCC_CFGR_SW_HSE << RCC_CFGR_SW_SHIFT);
    // // Select source SYSCLK = PLL
	// RCC_CFGR |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_SHIFT);
    // // Wait till PLL is used
	// while((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {}


    // RCC_PLLSAICFGR |= 192 << RCC_PLLSAICFGR_PLLSAIN_SHIFT;
	// RCC_PLLSAICFGR |= 3   << RCC_PLLSAICFGR_PLLSAIR_SHIFT;
	// RCC_PLLSAICFGR |= 2   << RCC_PLLSAICFGR_PLLSAIQ_SHIFT;
	// RCC_DCKCFGR1 &= ~(RCC_DCKCFGR1_PLLSAIDIVR_MASK << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT);
	
	// /* wait till RCC configured */
	// RCC_CR |= RCC_CR_PLLSAION;
	// while ((RCC_CR & RCC_CR_PLLSAIRDY) == 0) ;
}


#endif