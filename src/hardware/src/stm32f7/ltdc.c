#if defined (STM32F7)

#include "ltdc.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/ltdc.h>
#include <libopencm3/cm3/nvic.h>
#include "digitalpin.h"
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>


#define HSYNC       20
#define HBP         140
#define HFP         32

#define VSYNC        3
#define VBP          20
#define VFP          12

typedef struct {
	uint32_t pixFormat;
	size_t pixSize;
} colormodel_descript;

static const colormodel_descript LTDC_COLOR_MODELS[] = {
	{ LTDC_LxPFCR_ARGB8888, sizeof(uint32_t) },
	{ LTDC_LxPFCR_ARGB4444, sizeof(uint16_t) },
};

static void * FRAMEBUFFER_1;
static void * FRAMEBUFFER_2;

typedef struct {
	uint32_t gpio_port;
	uint32_t gpio_rcc;
	uint32_t gpio_pins;
	uint8_t  gpio_af;
} ltdc_pins_descript;

static const ltdc_pins_descript LTDC_PINS[] = {
	{ GPIOB, RCC_GPIOB, GPIO1, GPIO_AF9 },
	{ GPIOC, RCC_GPIOC, GPIO0, GPIO_AF14 },
	{ GPIOD, RCC_GPIOD, GPIO6, GPIO_AF14 },
	{ GPIOE, RCC_GPIOE, GPIO4 | GPIO5 | GPIO6, GPIO_AF14 },
	{ GPIOF, RCC_GPIOF, GPIO10, GPIO_AF14 },
	{ GPIOG, RCC_GPIOG, GPIO10, GPIO_AF9 },
	{ GPIOG, RCC_GPIOG, GPIO6 | GPIO7 | GPIO11 | GPIO12, GPIO_AF14 },
	{ GPIOH, RCC_GPIOH, GPIO2 | GPIO3 | GPIO8 | GPIO9 | 
						GPIO10 | GPIO13 | GPIO15, GPIO_AF14 },
	{ GPIOI, RCC_GPIOI, GPIO0 | GPIO1 | GPIO2 | GPIO4 | GPIO5 |
						GPIO6 | GPIO7 | GPIO9 | GPIO10, GPIO_AF14 },
};
#define LTDC_PINS_SIZE sizeof(LTDC_PINS)/sizeof(LTDC_PINS[0])

void ltdc_init(const LTDC_Layer * l1, const LTDC_Layer * l2) {
	/* my heap is located in sdram (look sbrk.c) */
	FRAMEBUFFER_1 = malloc(LTDC_SIZE * LTDC_COLOR_MODELS[l1->m].pixSize);
	FRAMEBUFFER_2 = malloc(LTDC_SIZE * LTDC_COLOR_MODELS[l2->m].pixSize);

	for (uint8_t i = 0; i < LTDC_PINS_SIZE; ++i) {
		/* init GPIO clocks */
		rcc_periph_clock_enable(LTDC_PINS[i].gpio_rcc);
		/* set GPIO pin modes */
		gpio_mode_setup(LTDC_PINS[i].gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_PINS[i].gpio_pins);
		gpio_set_output_options(LTDC_PINS[i].gpio_port, 
			GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, LTDC_PINS[i].gpio_pins);
		gpio_set_af(LTDC_PINS[i].gpio_port, LTDC_PINS[i].gpio_af, LTDC_PINS[i].gpio_pins);
	}

	/* max led_pwm */
	digitalpin_mode(PA_3, DIGITALPIN_OUTPUT);
	digitalpin_set(PA_3, 0);

	/* configure rcc pll clocking */
	RCC_PLLSAICFGR |= 192 << RCC_PLLSAICFGR_PLLSAIN_SHIFT;
	RCC_PLLSAICFGR |= 3   << RCC_PLLSAICFGR_PLLSAIR_SHIFT;
	RCC_PLLSAICFGR |= 2   << RCC_PLLSAICFGR_PLLSAIQ_SHIFT;
	RCC_DCKCFGR1 &= ~(RCC_DCKCFGR1_PLLSAIDIVR_MASK << RCC_DCKCFGR1_PLLSAIDIVR_SHIFT);
	/* wait till RCC configured */
	RCC_CR |= RCC_CR_PLLSAION;
	while ((RCC_CR & RCC_CR_PLLSAIRDY) == 0) ;
	
	/*
	 * Configure the Synchronous timings: VSYNC, HSNC,
	 * Vertical and Horizontal back porch, active data area, and
	 * the front porch timings.
	 */
	LTDC_SSCR = (HSYNC - 1) << LTDC_SSCR_HSW_SHIFT |
		    (VSYNC - 1) << LTDC_SSCR_VSH_SHIFT;
	LTDC_BPCR = (HSYNC + HBP - 1) << LTDC_BPCR_AHBP_SHIFT |
		    (VSYNC + VBP - 1) << LTDC_BPCR_AVBP_SHIFT;
	LTDC_AWCR = (HSYNC + HBP + LTDC_WIDTH - 1) << LTDC_AWCR_AAW_SHIFT |
		    (VSYNC + VBP + LTDC_HEIGHT - 1) << LTDC_AWCR_AAH_SHIFT;
	LTDC_TWCR =
	    (HSYNC + HBP + LTDC_WIDTH + HFP - 1) << LTDC_TWCR_TOTALW_SHIFT |
	    (VSYNC + VBP + LTDC_HEIGHT + VFP - 1) << LTDC_TWCR_TOTALH_SHIFT;

	/* Configure the synchronous signals and clock polarity. */
	LTDC_GCR |= LTDC_GCR_PCPOL_ACTIVE_HIGH;


	/* Configure the Layer 1 parameters.
	 * (Layer 1 is the bottom l->layerN.)    */
	if (l1) {
		ltdc_setLayer(l1);
		/* The color frame buffer start address */
		LTDC_L1CFBAR = (uint32_t)FRAMEBUFFER_1;
		/* Enable Layer1 and if needed the CLUT */
		LTDC_L1CR |= LTDC_LxCR_LAYER_ENABLE;
	}

	/* Configure the Layer 2 parameters. */
	if (l2) {
		ltdc_setLayer(l2);
		/* The color frame buffer start address */
		LTDC_L2CFBAR = (uint32_t)FRAMEBUFFER_2;
		/* Enable Layer2 and if needed the CLUT */
		LTDC_L2CR |= LTDC_LxCR_LAYER_ENABLE;
	}

	ltdc_setBackground(0);

	/* Configure the needed interrupts. */
	LTDC_IER = LTDC_IER_RRIE;
	nvic_enable_irq(NVIC_LCD_TFT_IRQ);
	/* Reload the shadow registers to active registers. */
	LTDC_SRCR |= LTDC_SRCR_VBR;
	/* Enable the LTDC-TFT controller. */
	LTDC_GCR |= LTDC_GCR_LTDC_ENABLE;

}

void ltdc_setLayer(const LTDC_Layer * l) {
	/* The pixel input format */
	LTDC_LxPFCR(l->layerN) = LTDC_COLOR_MODELS[l->m].pixFormat;

	/* x shift */
	uint32_t h_start = HSYNC + HBP + l->x;
	uint32_t h_stop = h_start + l->width - 1;
	LTDC_LxWHPCR(l->layerN) = h_stop << LTDC_LxWHPCR_WHSPPOS_SHIFT |
		       h_start << LTDC_LxWHPCR_WHSTPOS_SHIFT;
	/* y shift */
	uint32_t v_start = VSYNC + VBP + l->y;
	uint32_t v_stop = v_start + l->height - 1;
	LTDC_LxWVPCR(l->layerN) = v_stop << LTDC_LxWVPCR_WVSPPOS_SHIFT |
		       v_start << LTDC_LxWVPCR_WVSTPOS_SHIFT;

	/* The line length and pitch of the color frame buffer */
	uint32_t pitch = l->width * LTDC_COLOR_MODELS[l->m].pixSize;
	LTDC_LxCFBLR(l->layerN) = pitch << LTDC_LxCFBLR_CFBP_SHIFT |
				(pitch+3) << LTDC_LxCFBLR_CFBLL_SHIFT;

	/* The number of lines of the color frame buffer */
	LTDC_LxCFBLNR(l->layerN) = l->height;

	/* If needed, configure blendingfactors */
	LTDC_LxBFCR(l->layerN) = LTDC_LxBFCR_BF1_PIXEL_ALPHA_x_CONST_ALPHA |
				LTDC_LxBFCR_BF2_PIXEL_ALPHA_x_CONST_ALPHA;
	/* transparency */
	LTDC_LxCACR(l->layerN) = l->transp;
}

void ltdc_setBackground(uint32_t color) {
	LTDC_BCCR = color;
}

void * ltdc_getFramebuf(const LTDC_Layer * l) {
	return (l->layerN == 1) ? FRAMEBUFFER_1 : FRAMEBUFFER_2;
}

void ltdc_setPixel(const LTDC_Layer * l, uint16_t x, uint16_t y, uint32_t color) {
	if ( (x >= l->width) || (y >= l->height) ) return;
	const size_t i = (y+1) * l->width - x;
	const uint32_t bites = LTDC_COLOR_MODELS[l->m].pixSize * 8;
	const uint32_t mask = UINT32_MAX >> (32 - bites);

	void * buf = ltdc_getFramebuf(l);
	uint32_t * addr = (uint32_t *)(buf + i * LTDC_COLOR_MODELS[l->m].pixSize);
	*addr &= ~mask;
	*addr |= color & mask;
}

void lcd_tft_isr(void)
{
	LTDC_ICR |= LTDC_ICR_CRRIF;
	ltdc_handler();
	LTDC_SRCR |= LTDC_SRCR_VBR;
}

static void null_handler(void) {;}
#pragma weak ltdc_handler = null_handler

#endif