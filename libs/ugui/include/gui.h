#ifndef _UGUI_LIB_GUI_H_
#define _UGUI_LIB_GUI_H_

#include "../src/ugui.h"
#include "controls/ssd1306.h"


void ugui_init(SSD1306 * oled);

void ugui_flush(void);

void ugui_putString(int16_t x, int16_t y, const char* str);

#endif
