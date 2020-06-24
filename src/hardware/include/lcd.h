#ifndef __HARDWARE_LCD_H__
#define __HARDWARE_LCD_H__

#include <stdint.h>

typedef enum
{
	ARGB8888,
    ARGB4444,
    RGB888,
    RGB565,
} LCD_COLOR_MODEL;

typedef struct {
    uint8_t layerN;
	const LCD_COLOR_MODEL m;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
    uint32_t transp;
} LCD_Layer;

#define LCD_WIDTH  240
#define LCD_HEIGHT 320
#define LCD_SIZE   (LCD_WIDTH * LCD_HEIGHT)

void lcd_init(const LCD_Layer * l1, const LCD_Layer * l2);

void lcd_setBackground(uint32_t color);

void lcd_setLayer(const LCD_Layer * l);

void lcd_setPixel(const LCD_Layer * l, uint16_t x, uint16_t y, uint32_t color);


void lcd_handler(void);

#endif
