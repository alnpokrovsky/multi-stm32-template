#ifndef __CONTROLS_SSD1306_H__
#define __CONTROLS_SSD1306_H__

#include <stdint.h>
#include <stdbool.h>
#include "spibus.h"
#include "gpio.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

/** oled display connection
 * miso -> none
 * mosi -> sda(d1)
 * sck -> sck(d0)
 * nrst -> res
 */
typedef struct {
    SPIBUS_PORT spi;
    GPIO_Pins cs; /* chip select */
    GPIO_Pins dc; /* DC-pin switches data/command mode */ 
    bool isNeedRedraw;
    uint8_t buffer[SSD1306_WIDTH][SSD1306_HEIGHT/8];
} SSD1306;

void ssd1306_init(SSD1306 * oled);

void ssd1306_draw_point(SSD1306 * oled, int16_t x, int16_t y, uint8_t brightness);

void ssd1306_clear(SSD1306 * oled);

void ssd1306_flush(SSD1306 * oled);

#endif