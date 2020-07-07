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
#define HFP         160

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
	{ LTDC_LxPFCR_RGB888, 	3 },
	{ LTDC_LxPFCR_RGB565, 	sizeof(uint16_t) },
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

void ltdc_init(void) {

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

	/* enable RCC */
	RCC_APB2ENR |= RCC_APB2ENR_LTDCEN;
	
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
	LTDC_GCR |= LTDC_GCR_VSPOL_ACTIVE_LOW;
	LTDC_GCR |= LTDC_GCR_HSPOL_ACTIVE_LOW;
	LTDC_GCR |= LTDC_GCR_DEPOL_ACTIVE_LOW;

	ltdc_setBackground(0);

	/* Configure the needed interrupts. */
	LTDC_IER |= LTDC_IER_LIE; // line interrupt
	LTDC_LIPCR |= (VSYNC + VBP + LTDC_HEIGHT - 1) << LTDC_LIPCR_LIPOS_SHIFT; // line number
	// nvic_enable_irq(NVIC_LCD_TFT_IRQ);

	/* Reload the shadow registers to active registers. */
	LTDC_SRCR |= LTDC_SRCR_VBR;
	/* Enable the LTDC-TFT controller. */
	LTDC_GCR |= LTDC_GCR_LTDC_ENABLE;

}

void ltdc_setLayer(const LTDC_Layer * l) {
	if (!l->enable) {
		LTDC_LxCR(l->layerN) &= ~LTDC_LxCR_LAYER_ENABLE;
	} else {
		/* Enable Layer and if needed the CLUT */
		LTDC_LxCR(l->layerN) |= LTDC_LxCR_LAYER_ENABLE;

		/* The pixel input format */
		LTDC_LxPFCR(l->layerN) = LTDC_COLOR_MODELS[l->cm].pixFormat;

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
		uint32_t pitch = l->width * LTDC_COLOR_MODELS[l->cm].pixSize;
		LTDC_LxCFBLR(l->layerN) = pitch << LTDC_LxCFBLR_CFBP_SHIFT |
					(pitch+3) << LTDC_LxCFBLR_CFBLL_SHIFT;

		/* The number of lines of the color frame buffer */
		LTDC_LxCFBLNR(l->layerN) = l->height;

		/* If needed, configure blendingfactors */
		LTDC_LxBFCR(l->layerN) = LTDC_LxBFCR_BF1_PIXEL_ALPHA_x_CONST_ALPHA |
					LTDC_LxBFCR_BF2_PIXEL_ALPHA_x_CONST_ALPHA;
		/* transparency */
		LTDC_LxCACR(l->layerN) = l->transp;

		/* framebuffer */
		LTDC_LxCFBAR(l->layerN) = (uint32_t)l->framebuf;
	}

	/* Reload the shadow registers to active on Vsync */
	LTDC_SRCR |= LTDC_SRCR_VBR;
}

/**
 * wait till next frame ready
 */
void ltdc_waitVSync(void) {
	while (LTDC_SRCR_IS_RELOADING()) ;
	while ( !(LTDC_CDSR & LTDC_CDSR_VSYNCS) );
}

void ltdc_setBackground(uint32_t color) {
	LTDC_BCCR = color;
}

static void * ltdc_getPixelAddr(const LTDC_Layer * l, uint16_t x, uint16_t y) {
	uint32_t offset = (y * l->width + x) * LTDC_COLOR_MODELS[l->cm].pixSize;
	uint32_t buf = (uint32_t)((l->layerN == 1) ? FRAMEBUFFER_1 : FRAMEBUFFER_2);
	return (void*)(buf + offset);
}

void ltdc_setPixel(const LTDC_Layer * l, uint16_t x, uint16_t y, uint32_t color) {
	if ( (x >= l->width) || (y >= l->height) ) return;
	// x = l->width - x; // mirroring x
	const uint32_t bites = LTDC_COLOR_MODELS[l->cm].pixSize * 8;
	const uint32_t mask = UINT32_MAX << bites;

	uint32_t * addr = ltdc_getPixelAddr(l, x, y);
	*addr &= mask;
	*addr |= color & mask;
}

#include "sramfunc.h"
void SRAM_FUNC lcd_tft_isr(void)
{
	LTDC_ICR |= LTDC_ICR_CRRIF;
	ltdc_handler();
	LTDC_SRCR |= LTDC_SRCR_VBR;
}

static void null_handler(void) {;}
#pragma weak ltdc_handler = null_handler

#endif
