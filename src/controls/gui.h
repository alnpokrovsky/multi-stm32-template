#ifndef _CONTROLS_GUI_H_
#define _CONTROLS_GUI_H_

#ifdef LVGL_LIB
#include <lvgl.h>
#else
#error "you didn't include lvgl lib"
#endif


void gui_init(void);

void gui_startPolling(void);

#endif
