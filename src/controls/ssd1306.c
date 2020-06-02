#include "ssd1306.h"
#include "spi.h"
#include "digitalpin.h"
#include <string.h>

static void set_bit_brightness(
    uint8_t * point, 
    uint8_t bit, 
    uint8_t brightness
) {
    if (brightness == 0) { // black
        *point &= ~(1 << bit);
    } else { //white
        *point |= (1 << bit);
    }
}

static void command(SSD1306 * oled, uint8_t b) {
    digitalpin_set(oled->dc, 0);
    spi_xtransfer(oled->spi, b);
}

static void data(SSD1306 * oled, uint8_t b) {
    digitalpin_set(oled->dc, 1);
    spi_xtransfer(oled->spi, b);
}

void ssd1306_init(SSD1306 * oled) {
    // init periph
    spi_init(SPI_1, DFF_8BIT);
    digitalpin_mode(oled->dc, DIGITALPIN_OUTPUT);
    digitalpin_mode(oled->cs, DIGITALPIN_OUTPUT);
    digitalpin_set(oled->cs, 1);

    // initiation commands
    digitalpin_set(oled->cs, 0);
    static const uint8_t INIT_COMMANDS[] = {
		0xAE, 0x00, 0x10, 0x40, 0x81, 0xCF, 0xA1, 0xA6, 
		0xA8, 0x3F, 0xD3, 0x00, 0xD5, 0x80, 0xD9, 0xF1, 
        0xDA, 0x12, 0xDB, 0x40, 0x8D, 0x14, 0xAF, 0xFF
    };
    for (int i = 0; INIT_COMMANDS[i] != 0xFF; ++i) {
        command(oled, INIT_COMMANDS[i]);
    }
    digitalpin_set(oled->cs, 1);

    // init draw
    ssd1306_clear(oled);
    ssd1306_flush(oled);
}

void ssd1306_draw_point(SSD1306 * oled, int16_t x, int16_t y, uint8_t brightness) {
    // check if out of range
    if ( (x < 0) || (x >= SSD1306_WIDTH) ) return;
    if ( (y < 0) || (y >= SSD1306_HEIGHT) ) return;
    
    y = SSD1306_HEIGHT-1 - y; // invert y axis
    set_bit_brightness(&oled->buffer[x][y>>3], y & 0x07, brightness);
    oled->isNeedRedraw = true;
}

void ssd1306_clear(SSD1306 * oled) {
    oled->isNeedRedraw = true;
    memset(oled->buffer, 0, sizeof(oled->buffer));
}

void ssd1306_flush(SSD1306 * oled) {
    if (!oled->isNeedRedraw) return;
    oled->isNeedRedraw = false;

    digitalpin_set(oled->cs, 0);

    command(oled, 0x20); command(oled, 0x02); //page mode
    command(oled, 0x40);
    command(oled, 0xD3); command(oled, 0x00);
    for (int px=0; px < SSD1306_HEIGHT/8; ++px) {
        command(oled, 0xB0|px);
        command(oled, 0x00); // Lo col
        command(oled, 0x10); // Hi col
        for (int bx=0; bx < SSD1306_WIDTH; ++bx)
        data(oled, oled->buffer[bx][px]);
    }
    
    digitalpin_set(oled->cs, 1);
}
