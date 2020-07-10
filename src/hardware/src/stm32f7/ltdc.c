#if defined (STM32F7)

#include "ltdc.h"
#include <stm32f7xx.h>
#include <stm32f7xx_it.h>
#include "gpio.h"
#include <stdlib.h>


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

/* LTDC_LxPFCR - Pixel formats */
#define LTDC_LxPFCR_ARGB8888       (0b000)
#define LTDC_LxPFCR_RGB888         (0b001)
#define LTDC_LxPFCR_RGB565         (0b010)
#define LTDC_LxPFCR_ARGB1555       (0b011)
#define LTDC_LxPFCR_ARGB4444       (0b100)
#define LTDC_LxPFCR_L8             (0b101)
#define LTDC_LxPFCR_AL44           (0b110)
#define LTDC_LxPFCR_AL88           (0b111)
/* LTDC_LxBFCR - Blending factors - BF1 */
#define LTDC_LxBFCR_BF1_CONST_ALPHA               (0b100)
#define LTDC_LxBFCR_BF1_PIXEL_ALPHA_x_CONST_ALPHA (0b110)
/* LTDC_LxBFCR - Blending factors - BF2 */
#define LTDC_LxBFCR_BF2_CONST_ALPHA               (0b101)
#define LTDC_LxBFCR_BF2_PIXEL_ALPHA_x_CONST_ALPHA (0b111)
/* layer selector */
#define LTDC_Layer(l) ( l==1 ? LTDC_Layer1 : LTDC_Layer2 )


static const colormodel_descript LTDC_COLOR_MODELS[] = {
	{ LTDC_LxPFCR_ARGB8888, sizeof(uint32_t) },
	{ LTDC_LxPFCR_ARGB4444, sizeof(uint16_t) },
	{ LTDC_LxPFCR_RGB888, 	3 },
	{ LTDC_LxPFCR_RGB565, 	sizeof(uint16_t) },
};

static const GPIO_Pins LTDC_PINS[] = {
	{
		GPIO_PORT_B, GPIO_PIN_1, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 9
	},
	{
		GPIO_PORT_C, GPIO_PIN_0, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_D, GPIO_PIN_6, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_E, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_F, GPIO_PIN_10, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_G, GPIO_PIN_10, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 9
	},
	{
		GPIO_PORT_G, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11 | GPIO_PIN_12, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_H, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_15, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
	{
		GPIO_PORT_I, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10, 
		GPIO_PUPD_NONE, GPIO_MODE_AF, 14
	},
};
#define LTDC_PINS_SIZE sizeof(LTDC_PINS)/sizeof(LTDC_PINS[0])

void ltdc_init(void) {

	for (uint8_t i = 0; i < LTDC_PINS_SIZE; ++i) {
		gpio_init(&LTDC_PINS[i]);
	}

	/* enable RCC */
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;
	
	/*
	 * Configure the Synchronous timings: VSYNC, HSNC,
	 * Vertical and Horizontal back porch, active data area, and
	 * the front porch timings.
	 */
	LTDC->SSCR = (HSYNC - 1) << LTDC_SSCR_HSW_Pos |
		    (VSYNC - 1) << LTDC_SSCR_VSH_Pos;
	LTDC->BPCR = (HSYNC + HBP - 1) << LTDC_BPCR_AHBP_Pos |
		    (VSYNC + VBP - 1) << LTDC_BPCR_AVBP_Pos;
	LTDC->AWCR = (HSYNC + HBP + LTDC_WIDTH - 1) << LTDC_AWCR_AAW_Pos |
		    (VSYNC + VBP + LTDC_HEIGHT - 1) << LTDC_AWCR_AAH_Pos;
	LTDC->TWCR =
	    (HSYNC + HBP + LTDC_WIDTH + HFP - 1) << LTDC_TWCR_TOTALW_Pos |
	    (VSYNC + VBP + LTDC_HEIGHT + VFP - 1) << LTDC_TWCR_TOTALH_Pos;

	/**
	 * Configure the synchronous signals and clock polarity.
	 * 1 - active high; 0 - active low
	 */
	LTDC->GCR |= LTDC_GCR_PCPOL;
	LTDC->GCR &= ~LTDC_GCR_DEPOL;
	LTDC->GCR &= ~LTDC_GCR_VSPOL;
	LTDC->GCR &= ~LTDC_GCR_HSPOL;

	ltdc_setBackground(0xffff0000);

	/* Configure the needed interrupts. */
	LTDC->IER |= LTDC_IER_LIE; // line interrupt
	LTDC->LIPCR |= (VSYNC + VBP + LTDC_HEIGHT + VFP - 1) << LTDC_LIPCR_LIPOS_Pos; // line number
	// LTDC_LIPCR |= LTDC_HEIGHT << LTDC_LIPCR_LIPOS_Pos; // line number

	/* Reload the shadow registers to active registers. */
	LTDC->SRCR |= LTDC_SRCR_VBR;
	/* Enable the LTDC-TFT controller. */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

	/* max led_pwm */
	GPIO_Pins pa_3 = { GPIO_PORT_A, GPIO_PIN_3, GPIO_PUPD_NONE, GPIO_MODE_OUTPUT, 0x00 };
	gpio_init(&pa_3);
	gpio_set(&pa_3, GPIO_PINS_OFF);
}

void ltdc_setInterrupt(bool enable) {
	if (enable) {
		NVIC_EnableIRQ(LTDC_IRQn);
	} else {
		NVIC_DisableIRQ(LTDC_IRQn);
	}
}

void ltdc_setLayer(const LTDC_Layer * l) {
	if (!l->enable) {
		LTDC_Layer(l->layerN)->CR &= ~LTDC_LxCR_LEN;
	} else {
		/* Enable Layer and if needed the CLUT */
		LTDC_Layer(l->layerN)->CR |= LTDC_LxCR_LEN;

		/* The pixel input format */
		LTDC_Layer(l->layerN)->PFCR = LTDC_COLOR_MODELS[l->cm].pixFormat;

		/* x shift */
		uint32_t h_start = HSYNC + HBP + l->x;
		uint32_t h_stop = h_start + l->width - 1;
		LTDC_Layer(l->layerN)->WHPCR = h_stop << LTDC_LxWHPCR_WHSPPOS_Pos |
								 	  h_start << LTDC_LxWHPCR_WHSTPOS_Pos;
		/* y shift */
		uint32_t v_start = VSYNC + VBP + l->y;
		uint32_t v_stop = v_start + l->height - 1;
		LTDC_Layer(l->layerN)->WVPCR = v_stop << LTDC_LxWVPCR_WVSPPOS_Pos |
									  v_start << LTDC_LxWVPCR_WVSTPOS_Pos;

		/* The line length and pitch of the color frame buffer */
		uint32_t pitch = l->width * LTDC_COLOR_MODELS[l->cm].pixSize;
		LTDC_Layer(l->layerN)->CFBLR = pitch << LTDC_LxCFBLR_CFBP_Pos |
								   (pitch+3) << LTDC_LxCFBLR_CFBLL_Pos;

		/* The number of lines of the color frame buffer */
		LTDC_Layer(l->layerN)->CFBLNR = l->height;

		/* If needed, configure blendingfactors */
		LTDC_Layer(l->layerN)->BFCR = LTDC_LxBFCR_BF1_PIXEL_ALPHA_x_CONST_ALPHA << LTDC_LxBFCR_BF1_Pos;
		LTDC_Layer(l->layerN)->BFCR = LTDC_LxBFCR_BF2_PIXEL_ALPHA_x_CONST_ALPHA << LTDC_LxBFCR_BF2_Pos;
		/* transparency */
		LTDC_Layer(l->layerN)->CACR = l->transp;

		/* framebuffer */
		LTDC_Layer(l->layerN)->CFBAR = (uint32_t)l->framebuf;
	}

	/* Reload the shadow registers to active on Vsync */
	LTDC->SRCR |= LTDC_SRCR_VBR;
}

/**
 * wait till next frame ready
 */
void ltdc_waitVSync(void) {
	while ( !(LTDC->CDSR & LTDC_CDSR_VSYNCS) );
}

void ltdc_setBackground(uint32_t color) {
	LTDC->BCCR = color;
}

static void * ltdc_getPixelAddr(const LTDC_Layer * l, uint16_t x, uint16_t y) {
	uint32_t offset = (y * l->width + x) * LTDC_COLOR_MODELS[l->cm].pixSize;
	uint32_t buf = (uint32_t)(l->framebuf);
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
void SRAM_FUNC LTDC_IRQHandler(void)
{
	ltdc_handler();
	LTDC->ICR |= LTDC_ICR_CLIF;
}

__attribute__((weak))
void ltdc_handler(void) {;}

#endif
