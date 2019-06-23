#include "ssd1306.h"
#include "spi.h"
#include "digitalpin.h"
#include "ugui.h"
#include <assert.h>

#define WIDTH 128
#define HEIGHT 64

static UG_GUI gui;
static uint8_t buffer[WIDTH / sizeof(uint8_t)][HEIGHT];
static struct {
    SPI_PORT spi;
    DIGITAL_PIN dc; /* DC-pin switches data/command mode */ 
} SSD1306;

static void set_bit_color(
    uint8_t * point, 
    uint8_t bit, 
    UG_COLOR c
) {
    switch ( c ) {
	case C_BLACK: // black
		*point &= ~(1 << bit);
        break;
	case C_RED: // inverse
        *point ^= (1 << bit);
        break;
    case C_WHITE: // white
	default:
        *point |= (1 << bit);
        break;
	}
}

static void draw_point(UG_S16 x, UG_S16 y, UG_COLOR c) {
    assert(x > 0 && x < WIDTH);
    assert(y > 0 && y < HEIGHT);
    set_bit_color(&buffer[x >> 3][y], x & 0x07, c);
}

static void command(uint8_t b) {
    digitalpin_set(SSD1306.dc, 0);
    spi_xtransfer(SSD1306.spi, b);
}

static void data(uint8_t b) {
    digitalpin_set(SSD1306.dc, 1);
    spi_xtransfer(SSD1306.spi, b);
}

void ssd1306_init(SPI_PORT port, DIGITAL_PIN dc) {
    SSD1306.spi = port;
    SSD1306.dc = dc;
    digitalpin_mode(DC, DIGITALPIN_OUTPUT);
    UG_Init(&gui, draw_point, WIDTH, HEIGHT);
    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_4X6);
	UG_FontSetHSpace(0);
}

void ssd1306_clear(void) {
    memset(buffer, 0, sizeof(buffer));
}

void ssd1306_redraw(void) {
}