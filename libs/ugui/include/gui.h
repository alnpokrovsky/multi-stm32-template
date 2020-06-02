#ifndef _UGUI_LIB_GUI_H_
#define _UGUI_LIB_GUI_H_

#include "../src/ugui.h"
#include "controls/ssd1306.h"


void gui_init(void);

void gui_poll(void);

void gui_putString(int16_t x, int16_t y, const char* str);

#endif
