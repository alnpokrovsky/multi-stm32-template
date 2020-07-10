#if defined(STM32F7)

#include "rcc.h"
#include <stm32f7xx.h>
#include <stm32f7xx_it.h>
#include "pwr.h"
#include "reg.h"


#define RCC_DCKCFGR1_PLLSAIDIVR_DIVR_2		0
#define RCC_DCKCFGR1_PLLSAIDIVR_DIVR_4		1
#define RCC_DCKCFGR1_PLLSAIDIVR_DIVR_8		2
#define RCC_DCKCFGR1_PLLSAIDIVR_DIVR_16		3


void rcc_init() {
	// const struct rcc_clock_scale rcc = {
	// 	.plln = 400,
	// 	.pllp = 2,
	// 	.pllq = 8,
	// 	.hpre = RCC_CFGR_HPRE_DIV_NONE,
	// 	.ppre1 = RCC_CFGR_PPRE_DIV_4,
	// 	.ppre2 = RCC_CFGR_PPRE_DIV_2,
	// 	.vos_scale = PWR_SCALE1,
	// 	.overdrive = 1,
	// 	.flash_waitstates = 6,
	// 	.ahb_frequency = 200000000,
	// 	.apb1_frequency = 50000000,
	// 	.apb2_frequency = 100000000,
	// };

	uint32_t m,n,p,q,r;

	/* Enable internal high-speed oscillator. */
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	/* Select HSI as SYSCLK source. */
	REG_SUBSET(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI);

	/* Enable external high-speed oscillator. */
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) == 0);

	pwr_init(1); // max power
	pwr_setOverdrive(true);

	/*
	 * Set prescalers for AHB, ADC, APB1, APB2.
	 * Do this before touching the PLL (TODO: why?).
	 */
	REG_SUBSET(RCC->CFGR, RCC_CFGR_HPRE,  RCC_CFGR_HPRE_DIV1);
	REG_SUBSET(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
	REG_SUBSET(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);

	/* Disable PLL oscillator before changing its configuration. */
	RCC->CR &= ~RCC_CR_PLLON;
	/* Configure the PLL oscillator. */
	m = HSE_VALUE / 1000000; // this get us 1MHz on PLL input
	n = 400;
	p = 2;
	q = 8;
	RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE;
	RCC->PLLCFGR |= m 	<< RCC_PLLCFGR_PLLM_Pos;
	RCC->PLLCFGR |= n << RCC_PLLCFGR_PLLN_Pos;
	RCC->PLLCFGR |= ((p >> 1) - 1) 	<< RCC_PLLCFGR_PLLP_Pos;
	RCC->PLLCFGR |= q 	<< RCC_PLLCFGR_PLLQ_Pos;

	/* Disable internal high-speed oscillator. */
	RCC->CR &= ~RCC_CR_HSION;
	/* Enable PLL oscillator and wait for it to stabilize. */
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);

	/* Configure flash settings. */
	REG_SUBSET(FLASH->ACR, FLASH_ACR_LATENCY, 6);
	FLASH->ACR |= FLASH_ACR_ARTEN;
	FLASH->ACR |= FLASH_ACR_PRFTEN;

	/* Select PLL as SYSCLK source. */
	REG_SUBSET(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	/* Wait for PLL clock to be selected. */
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

	
    
	/* configure display and usb pll clocking */
	RCC->PLLSAICFGR = 0;
	n = 192;
	p = 4;
	q = 2;
	r = 2;
	RCC->PLLSAICFGR |= n << RCC_PLLSAICFGR_PLLSAIN_Pos;
	RCC->PLLSAICFGR |= ((p >> 1) - 1) << RCC_PLLSAICFGR_PLLSAIP_Pos;
	RCC->PLLSAICFGR |= q   << RCC_PLLSAICFGR_PLLSAIQ_Pos;
	RCC->PLLSAICFGR |= r   << RCC_PLLSAICFGR_PLLSAIR_Pos;
	RCC->DCKCFGR1 &= ~(RCC_DCKCFGR1_PLLSAIDIVR_Msk << RCC_DCKCFGR1_PLLSAIDIVR_Pos);
	RCC->DCKCFGR1 |= RCC_DCKCFGR1_PLLSAIDIVR_DIVR_2 << RCC_DCKCFGR1_PLLSAIDIVR_Pos;
	/* src for clk48 is pllsaip */
	RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
	/* wait till RCC configured */
	RCC->CR |= RCC_CR_PLLSAION;
	while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0) ;

	SystemCoreClockUpdate();
}


#endif