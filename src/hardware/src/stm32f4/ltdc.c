#if defined (STM32F4)

#include "ltdc.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/ltdc.h>
#include <libopencm3/cm3/nvic.h>
#include "dma2d.h"
#include "delay.h"
#include <stdint.h>
#include <malloc.h>


#define HSYNC       10
#define HBP         20
#define HFP         10

#define VSYNC        2
#define VBP          2
#define VFP          4

typedef struct {
	uint32_t pixFormat;
	uint32_t pixSize;
} colormodel_descript;

static const colormodel_descript LTDC_COLOR_MODELS[] = {
	{ LTDC_LxPFCR_ARGB8888, sizeof(uint32_t) },
	{ LTDC_LxPFCR_ARGB4444, sizeof(uint16_t) },
};

static void * FRAMEBUFFER_1;
static void * FRAMEBUFFER_2;

static void ltdc_command(uint8_t cmd, int delay, int n_args, const uint8_t *args)
{
	gpio_clear(GPIOC, GPIO2); /* Select the LTDC */
	while (!(SPI_SR(SPI5) & SPI_SR_TXE));
	spi_xfer(SPI5, cmd);
	
	if (n_args) {
		gpio_set(GPIOD, GPIO13); /* Set the D/CX pin */
		for (int i = 0; i < n_args; i++) {
			while (!(SPI_SR(SPI5) & SPI_SR_TXE));
			spi_xfer(SPI5, *(args+i));
		}
	}
	gpio_set(GPIOC, GPIO2);	   /* Turn off chip select */
	gpio_clear(GPIOD, GPIO13); /* always reset D/CX */
	
	delay_ms(delay); /* wait, if called for */
}


/* ILI9341 command definitions */

/* Regulative[sic] Command Set */
typedef enum {
	ILI_NOP =                 0x00,
	ILI_RESET =               0x01,
	ILI_RD_DID =              0x04,
	ILI_RD_STS =              0x09,
	ILI_RD_PWR_MODE =         0x0a,
	ILI_RD_MADCTL =           0x0b,
	ILI_RD_PXL_FMT =          0x0c,
	ILI_PD_IMG_FMT =          0x0d,
	ILI_RD_SIG_MODE =         0x0e,
	ILI_RD_DIAG_RSLT =        0x0f,
	ILI_ENTER_SLEEP =         0x10,
	ILI_SLEEP_OUT =           0x11,
	ILI_PARTIAL_ON =          0x12,
	ILI_NORMAL_MODE_ON =      0x13,
	ILI_INVERSE_ON =          0x20,
	ILI_INVERSE_OFF =         0x21,
	ILI_GAMMA_SET =           0x26,
	ILI_DISP_OFF =            0x28,
	ILI_DISP_ON =             0x29,
	ILI_CAS =                 0x2a,
	ILI_PAS =                 0x2b,
	ILI_MEM_WRITE =           0x2c,
	ILI_COLOR_SET =           0x2d,
	ILI_MEM_READ =            0x2e,
	ILI_PARTIAL_AREA =        0x30,
	ILI_VERT_SCROLL_DEF =     0x33,
	ILI_TEAR_EFF_OFF =        0x34,
	ILI_TEAR_EFF_ON =         0x35,
	ILI_MEM_ACC_CTL =         0x36,
	ILI_V_SCROLL_START =      0x37,
	ILI_IDLE_OFF =            0x38,
	ILI_IDLE_ON =             0x39,
	ILI_PIX_FMT_SET =         0x3a,
	ILI_WR_MEM_CONT =         0x3c,
	ILI_RD_MEM_CONT =         0x3e,
	ILI_SET_TEAR_LINE =       0x44,
	ILI_GET_SCANLINE =        0x45,
	ILI_WR_BRIGHTNESS =       0x51,
	ILI_RD_BRIGHTNESS =       0x52,
	ILI_WR_CTRL =             0x53,
	ILI_RD_CTRL =             0x54,
	ILI_WR_CABC =             0x55,
	ILI_RD_CABC =             0x56,
	ILI_WR_CABC_MIN =         0x5e,
	ILI_RD_CABC_MAX =         0x5f,
	ILI_RD_ID1 =              0xda,
	ILI_RD_ID2 =              0xdb,
	ILI_RD_ID3 =              0xdc,

	/* Extended Command Set */
	ILI_RGB_IFC_CTL =         0xb0,
	ILI_FRM_CTL_NORM =        0xb1,
	ILI_FRM_CTL_IDLE =        0xb2,
	ILI_FRM_CTL_PART =        0xb3,
	ILI_INVERSE_CTL =         0xb4,
	ILI_PORCH_CTL =           0xb5,
	ILI_FUNC_CTL =            0xb6,
	ILI_ENTRY_MODE_SET =      0xb7,
	ILI_BL_CTL_1 =            0xb8,
	ILI_BL_CTL_2 =            0xb9,
	ILI_BL_CTL_3 =            0xba,
	ILI_BL_CTL_4 =            0xbb,
	ILI_BL_CTL_5 =            0xbc,
	ILI_BL_CTL_7 =            0xbe,
	ILI_BL_CTL_8 =            0xbf,
	ILI_PWR_CTL_1 =           0xc0,
	ILI_PWR_CTL_2 =           0xc1,
	ILI_VCOM_CTL_1 =          0xc5,
	ILI_VCOM_CTL_2 =          0xc7,
	ILI_NV_MEM_WR =           0xd0,
	ILI_NV_MEM_PROT_KEY =     0xd1,
	ILI_NV_MEM_STATUS_RD =    0xd2,
	ILI_RD_ID4 =              0xd3,
	ILI_POS_GAMMA =           0xe0,
	ILI_NEG_GAMMA =           0xe1,
	ILI_GAMMA_CTL_1 =         0xe2,
	ILI_GAMMA_CTL_2 =         0xe3,
	ILI_IFC_CTL =             0xf6,
} ILI_COMMAND;

/*
 * This structure defines the sequence of commands to send
 * to the Display in order to initialize it. The AdaFruit
 * folks do something similar, it helps when debugging the
 * initialization sequence for the display.
 */

#define MAX_INLINE_ARGS (sizeof(uint8_t *))
struct tft_command {
	uint16_t delay;		/* If you need a delay after */
	uint8_t cmd;		/* command to send */
	uint8_t n_args;		/* How many arguments it has */
	union {
		uint8_t args[MAX_INLINE_ARGS]; /* The first four arguments */
		const uint8_t *aptr; /* More than four arguemnts */
	};
};

static const uint8_t pos_gamma_args[] = { 
	0x0F, 0x29, 0x24, 0x0C, 0x0E,
	0x09, 0x4E, 0x78, 0x3C, 0x09,
	0x13, 0x05, 0x17, 0x11, 0x00,
};
static const uint8_t neg_gamma_args[] = {
	0x00, 0x16, 0x1B, 0x04, 0x11,
	0x07, 0x31, 0x33, 0x42, 0x05,
	0x0C, 0x0A, 0x28, 0x2F, 0x0F,
};

/*
 * These are the commands we're going to send to the
 * display to initialize it. We send them all, in sequence
 * with occasional delays. Commands that require data bytes
 * as arguments, indicate how many bytes to pull out the
 * above array to include.
 *
 * The sequence was pieced together from the ST Micro demo
 * code, the data sheet, and other sources on the web.
 */
const struct tft_command INIT[] = {
	{  0, ILI_PWR_CTL_1,        1, .args = { 0x10 } },
	{  0, ILI_PWR_CTL_2,        1, .args = { 0x10 } },
	{  0, ILI_VCOM_CTL_1,       2, .args = { 0x45, 0x15 } },
	{  0, ILI_VCOM_CTL_2,       1, .args = { 0x90 } },
	{  0, ILI_MEM_ACC_CTL,      1, .args = { 0x48 } }, /* 0x48 - normal, 0x48 ^ 0xC0 - y mirrored, 0x08 - x mirrored */
	{  0, ILI_RGB_IFC_CTL,      1, .args = { 0xc0 } },
	{  0, ILI_IFC_CTL,          3, .args = { 0x01, 0x00, 0x06 } },
	{  0, ILI_GAMMA_SET,        1, .args = { 0x01 } },
	{  0, ILI_POS_GAMMA,       15, .aptr = pos_gamma_args },
	{  0, ILI_NEG_GAMMA,       15, .aptr = neg_gamma_args },
	{ +5, ILI_SLEEP_OUT,        0, .args = {} },
	{  0, ILI_DISP_ON,          0, .args = {} },
};
#define INIT_SIZE sizeof(INIT)/sizeof(INIT[0])


typedef struct {
	uint32_t gpio_port;
	uint32_t gpio_rcc;
	uint32_t gpio_pins;
	uint8_t  gpio_af;
} ltdc_pins_descript;

static const ltdc_pins_descript LTDC_PINS[] = {
	{ GPIOA, RCC_GPIOA, GPIO3 | GPIO4 | GPIO6 | GPIO11 | GPIO12, GPIO_AF14 },
	{ GPIOB, RCC_GPIOB, GPIO0 | GPIO1, GPIO_AF9 },
	{ GPIOB, RCC_GPIOB, GPIO8 | GPIO9 | GPIO10 | GPIO11, GPIO_AF14 },
	{ GPIOC, RCC_GPIOC, GPIO6 | GPIO7 | GPIO10, GPIO_AF14 },
	{ GPIOD, RCC_GPIOD, GPIO3 | GPIO6, GPIO_AF14 },
	{ GPIOF, RCC_GPIOF, GPIO10, GPIO_AF14 },
	{ GPIOG, RCC_GPIOG, GPIO10 | GPIO12, GPIO_AF9 },
	{ GPIOG, RCC_GPIOG, GPIO6 | GPIO7 | GPIO11, GPIO_AF14 },
};
#define LTDC_PINS_SIZE sizeof(LTDC_PINS)/sizeof(LTDC_PINS[0])


/**
 * Pin assignments
 *     R2      = PC10, AF14
 *     R3      = PB0,  AF09
 *     R4      = PA11, AF14
 *     R5      = PA12, AF14
 *     R6      = PB1,  AF09
 *     R7      = PG6,  AF14
 *
 *     G2      = PA6,  AF14
 *     G3      = PG10, AF09
 *     G4      = PB10, AF14
 *     G5      = PB11, AF14
 *     G6      = PC7,  AF14
 *     G7      = PD3,  AF14
 *
 *     B2      = PD6,  AF14
 *     B3      = PG11, AF11
 *     B4      = PG12, AF09
 *     B5      = PA3,  AF14
 *     B6      = PB8,  AF14
 *     B7      = PB9,  AF14
 *
 * More pins...
 *     ENABLE  = PF10, AF14
 *     DOTCLK  = PG7,  AF14
 *     HSYNC   = PC6,  AF14
 *     VSYNC   = PA4,  AF14
 *     CSX     = PC2         used in lcd-spi
 *     RDX     = PD12        not used: read SPI
 *     TE      = PD11        not used: tearing effect interrupt
 *     WRX_DCX = PD13        used in lcd-spi
 *     DCX_SCL = PF7         used in lcd-spi
 *     SDA     = PF9         used in lcd-spi
 *     NRST    = NRST
 */
void ltdc_init(const LTDC_Layer * l1, const LTDC_Layer * l2) {
	/* my heap is located in sdram (look sbrk.c) */
	FRAMEBUFFER_1 = malloc(LTDC_SIZE * LTDC_COLOR_MODELS[l1->cm].pixSize);
	FRAMEBUFFER_2 = malloc(LTDC_SIZE * LTDC_COLOR_MODELS[l2->cm].pixSize);

	for (uint8_t i = 0; i < LTDC_PINS_SIZE; ++i) {
		/* init GPIO clocks */
		rcc_periph_clock_enable(LTDC_PINS[i].gpio_rcc);
		/* set GPIO pin modes */
		gpio_mode_setup(LTDC_PINS[i].gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_PINS[i].gpio_pins);
		gpio_set_af(LTDC_PINS[i].gpio_port, LTDC_PINS[i].gpio_af, LTDC_PINS[i].gpio_pins);
	}

	/*
	 * The datasheet says (Figure 16, page 151):
	 *     The LTDC-TFT clock comes from PLLSAI.
	 *     PLLSRC selects either HSI or HSE.
	 *     PLLSAI's input clock is either HSI or HSE divided by PLLM.
	 *     PLLSAI's PLLLTDCCLK output is the input * PLLSAIN / PLLSAIR.
	 *     LTDC-TFT clock is PLLLTDCCLK divided by PLLSAIDIVR.
	 *
	 * PLLSRC and PLLM are in the RCC_PLLCFGR register.
	 * PLLSAIN and PLLSAIR are in RCC_PLLSAICFGR.
	 * PLLSAIDIVR is in RCC_DCKCFGR;
	 *
	 * In our case,
	 * PLLSRC already selected HSE, which is 8 MHz.
	 * PLLM is already set to 8.  8 MHz / 8 = 1 MHz.
	 * We set PLLSAIN = 192 and PLLSAIR = 4.  1 MHz * 192 / 4 = 48 MHz.
	 * We set PLLSAIDIVR to 8.  48 MHz / 8 = 6 MHz.
	 * So the LTDC-TFT pixel clock is 6 MHz.
	 *
	 * The number of clocks per frame is
	 * (VSYNC + VBP + LTDC_HEIGHT + VFP) * (HSYNC + HBP + LTDC_WIDTH + HFP) =
	 * (2 + 2 + 320 + 4) * (10 + 20 + 240 + 10) = 91840.
	 *
	 * So the refresh frequency is 6 MHz / 91840 ~= 65.6 Hz.
	 */
	RCC_PLLSAICFGR |= 192 << RCC_PLLSAICFGR_PLLSAIN_SHIFT;
	RCC_PLLSAICFGR |= 4   << RCC_PLLSAICFGR_PLLSAIR_SHIFT;
	RCC_DCKCFGR &= ~(RCC_DCKCFGR_PLLSAIDIVR_MASK << RCC_DCKCFGR_PLLSAIDIVR_SHIFT);
	RCC_DCKCFGR |= RCC_DCKCFGR_PLLSAIDIVR_DIVR_8 << RCC_DCKCFGR_PLLSAIDIVR_SHIFT;
	
	/* wait till RCC configured */
	RCC_CR |= RCC_CR_PLLSAION;
	while ((RCC_CR & RCC_CR_PLLSAIRDY) == 0) ;
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

	dma2d_init();
	
	/**
	 * Initialize the SPI port, and the through that port
	 * initialize the LTDC controller. Note that this code
	 * will expect to be able to draw into the SDRAM on
	 * the board, so the sdram much be initialized before
	 * calling this function.
	 *
	 * SPI Port and GPIO Defined - for STM32F4-Disco
	 *
	 * LTDC_CS      PC2
	 * LTDC_SCK     PF7
	 * LTDC_DC      PD13
	 * LTDC_MOSI    PF9
	 */
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOF);

	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO9);
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO9);

	rcc_periph_clock_enable(RCC_SPI5);
	spi_init_master(SPI5, SPI_CR1_BAUDRATE_FPCLK_DIV_4,
					SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1,
					SPI_CR1_DFF_8BIT,
					SPI_CR1_MSBFIRST);
	spi_enable_ss_output(SPI5);
	spi_enable(SPI5);

	/* Set up the display */
	for (size_t i = 0; i < INIT_SIZE; i++) {
		uint8_t arg_count = INIT[i].n_args;
		const uint8_t *args = (arg_count > MAX_INLINE_ARGS) ?
								 INIT[i].aptr : INIT[i].args;
		ltdc_command(INIT[i].cmd, INIT[i].delay, arg_count, args);
	}
}

void ltdc_setLayer(const LTDC_Layer * l) {
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
}

void ltdc_setBackground(uint32_t color) {
	LTDC_BCCR = color;
}

uint32_t * ltdc_getPixelAddr(const LTDC_Layer * l, uint16_t x, uint16_t y) {
	uint32_t offset = (y * l->width + x) * LTDC_COLOR_MODELS[l->cm].pixSize;
	void * buf = (l->layerN == 1) ? FRAMEBUFFER_1 : FRAMEBUFFER_2;
	return (uint32_t *)(buf + offset);
}

void ltdc_setPixel(const LTDC_Layer * l, uint16_t x, uint16_t y, uint32_t color) {
	if ( (x >= l->width) || (y >= l->height) ) return;
	// x = l->width - x; // mirroring x
	const uint32_t bites = LTDC_COLOR_MODELS[l->cm].pixSize * 8;
	const uint32_t mask = UINT32_MAX >> (32 - bites);

	uint32_t * addr = ltdc_getPixelAddr(l, x, y);
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
