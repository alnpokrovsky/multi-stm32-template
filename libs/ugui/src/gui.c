#include "gui.h"
#include "utf8_to_cp1251.h"
#include "controls/ssd1306.h"

static UG_GUI gui;
static SSD1306 oled = {.spi = SPI_1, .cs = PA_0, .dc = PA_1};

static void draw_point(UG_S16 x, UG_S16 y, UG_COLOR color) {
    ssd1306_draw_point(&oled, x, y, (uint8_t)color);
}

void gui_init(void) {
    ssd1306_init(&oled);
    ssd1306_clear(&oled);
    // uGUI setting
    UG_Init(&gui, &draw_point, SSD1306_WIDTH, SSD1306_HEIGHT);
    // UG_FillScreen(C_BLACK);    
    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_8X12);
	UG_FontSetHSpace(0);
}

void gui_clear(void) {
    ssd1306_clear(&oled);
}

void gui_putString(int16_t x, int16_t y, const char* str) {
    char tmp[32];
    convertUtf8ToCp1251(str, tmp);
    UG_PutString(x, y, tmp);
}

void gui_poll(void) {
    ssd1306_flush(&oled);
}
