#ifndef __HARDWARE_LTDC_H__
#define __HARDWARE_LTDC_H__

#include <stdint.h>

#if defined(STM32F4)
#define LTDC_WIDTH  240
#define LTDC_HEIGHT 320
#elif defined(STM32F7)
#define LTDC_WIDTH  1024
#define LTDC_HEIGHT 600
#else
#error "not supported"
#endif

#define LTDC_SIZE   (LTDC_WIDTH * LTDC_HEIGHT)

typedef enum
{
	ARGB8888,
    ARGB4444,
    RGB888,
    RGB565,
} LTDC_COLOR_MODEL;

typedef struct {
    uint8_t layerN;
	const LTDC_COLOR_MODEL cm;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
    uint32_t transp;
} LTDC_Layer;

void ltdc_init(const LTDC_Layer * l1, const LTDC_Layer * l2);

void ltdc_setBackground(uint32_t color);

void ltdc_setLayer(const LTDC_Layer * l);

uint32_t * ltdc_getPixelAddr(const LTDC_Layer * l, uint16_t x, uint16_t y);

void ltdc_setPixel(const LTDC_Layer * l, uint16_t x, uint16_t y, uint32_t color);


void ltdc_handler(void);

#endif