#if defined(STM32F7)

#include "sdram.h"
#include "delay.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/fsmc.h>

uint8_t * const _sdram = (uint8_t *)(0xd0000000);
uint8_t * const _esdram = (uint8_t *)(0xd0000000 + 8*1024*1024);

static const struct {
	uint32_t	gpio;
	uint32_t	rcc;
	uint16_t	pins;
} SDRAM_PINS[] = {
	{GPIOD, RCC_GPIOD, GPIO0 | GPIO1 | GPIO8 | GPIO9 | GPIO10 | GPIO14 | GPIO15},
	{GPIOE, RCC_GPIOE, GPIO0 | GPIO1 | GPIO7 | GPIO8 |
			GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15},
	{GPIOF, RCC_GPIOF, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 |
			GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15 },
	{GPIOG, RCC_GPIOG, GPIO0 | GPIO1 | GPIO4 | GPIO5 | GPIO8 | GPIO15},
	{GPIOH, RCC_GPIOH, GPIO5 | GPIO6 | GPIO7},
};
#define PINS_SIZE sizeof(SDRAM_PINS)/sizeof(SDRAM_PINS[0])

/**
 * 200/2=100MHz freq = 10ns one clock cycle
 * trcd min Trcd = 15ns = 2cycles
 * trp min Trp = 15ns = 2cycles
 * twr min Twr = 2cycles
 * trc min Trc = 60-63ns = 6-7 cycles
 * tras min Tdal = 2cycles + Trp = 4cycles
 * txsr min Txsr = 66-70ns = 7 cycles
 * tmrd min Tmrd  = 2 cycles
 * 
 */
static struct sdram_timing TIMING = {
	.trcd = 2,		/* RCD Delay */
	.trp = 2,		/* RP Delay */
	.twr = 2,		/* Write Recovery Time */
	.trc = 7,		/* Row Cycle Delay */
	.tras = 4,		/* Self Refresh Time */
	.txsr = 7,		/* Exit Self Refresh Time */
	.tmrd = 2,		/* Load to Active Delay */
};

void sdram_init(void) {
	/*
	* First all the GPIO pins that end up as SDRAM pins
	*/
	for (uint8_t i = 0; i < PINS_SIZE; i++) {
		rcc_periph_clock_enable(SDRAM_PINS[i].rcc);
		gpio_mode_setup(SDRAM_PINS[i].gpio, GPIO_MODE_AF,
				GPIO_PUPD_NONE, SDRAM_PINS[i].pins);
		gpio_set_output_options(SDRAM_PINS[i].gpio, GPIO_OTYPE_PP,
				GPIO_OSPEED_100MHZ, SDRAM_PINS[i].pins);
		gpio_set_af(SDRAM_PINS[i].gpio, GPIO_AF12, SDRAM_PINS[i].pins);
	}

	/* Enable the SDRAM Controller */
	rcc_periph_clock_enable(RCC_FMC);

	uint32_t cr_tmp  = FMC_SDCR_RPIPE_1CLK;
	cr_tmp |= FMC_SDCR_SDCLK_2HCLK;
	cr_tmp |= FMC_SDCR_CAS_2CYC;
	cr_tmp |= FMC_SDCR_NB4;
	cr_tmp |= FMC_SDCR_MWID_16b;
	cr_tmp |= FMC_SDCR_NR_12;
	cr_tmp |= FMC_SDCR_NC_8;
	cr_tmp |= FMC_SDCR_RBURST;

	/* We're programming BANK 2, but per the manual some of the parameters
	 * only work in CR1 and TR1 so we pull those off and put them in the
	 * right place.
	 */
	FMC_SDCR1 |= (cr_tmp & FMC_SDCR_DNC_MASK);
	FMC_SDCR2 = cr_tmp;

	uint32_t tr_tmp = sdram_timing(&TIMING);
	FMC_SDTR1 |= (tr_tmp & FMC_SDTR_DNC_MASK);
	FMC_SDTR2 = tr_tmp;

	/* Now start up the Controller per the manual
	 *	- Clock config enable
	 *	- PALL state
	 *	- set auto refresh
	 *	- Load the Mode Register
	 */
	sdram_command(SDRAM_BANK2, SDRAM_CLK_CONF, 1, 0);
	delay_ms(1); /* sleep at least 100mS */
	
	sdram_command(SDRAM_BANK2, SDRAM_PALL, 1, 0);
	sdram_command(SDRAM_BANK2, SDRAM_AUTO_REFRESH, 8, 0);
	tr_tmp = SDRAM_MODE_BURST_LENGTH_2				|
				SDRAM_MODE_BURST_TYPE_SEQUENTIAL	|
				SDRAM_MODE_CAS_LATENCY_2			|
				SDRAM_MODE_OPERATING_MODE_STANDARD	|
				SDRAM_MODE_WRITEBURST_MODE_SINGLE;
	sdram_command(SDRAM_BANK2, SDRAM_LOAD_MODE, 1, tr_tmp);

	/**
	 * sdramRefr/numberOfRows*cycleFreq - 20 
	 * 64ms/4096 * 100 - 20 = 1542.5
	 */
	FMC_SDRTR = 1543 << FMC_SDRTR_COUNT_SHIFT;
}


#endif
