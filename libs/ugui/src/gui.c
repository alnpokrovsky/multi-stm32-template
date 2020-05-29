#include "gui.h"
#include "utf8_to_cp1251.h"
#include "controls/ssd1306.h"

static UG_GUI gui;
static SSD1306 * oled = 0;

static void draw_point(UG_S16 x, UG_S16 y, UG_COLOR color) {
    ssd1306_draw_point(oled, x, y, color);
}

void ugui_init(SSD1306 * ssd1306) {
    oled = ssd1306;
    ssd1306_init(oled);
    ssd1306_clear(oled);
    // uGUI setting
    UG_Init(&gui, &draw_point, SSD1306_WIDTH, SSD1306_HEIGHT);
    // UG_FillScreen(C_BLACK);    
    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_8X12);
	UG_FontSetHSpace(0);
}

void ugui_putString(int16_t x, int16_t y, const char* str) {
    char tmp[32];
    convertUtf8ToCp1251(str, tmp);
    UG_PutString(x, y, tmp);
}

void ugui_flush(void) {
    ssd1306_flush(oled);
}
