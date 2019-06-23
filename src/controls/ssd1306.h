#ifndef __CONTROLS_SSD1306_H__
#define __CONTROLS_SSD1306_H__

/* oled display */

#include <stdint.h>
#include "spi.h"
#include "digitalpin.h"


void ssd1306_init(SPI_PORT port, DIGITALPIN_NAME dc);

void ssd1306_clear(void);

void ssd1306_redraw(void);

#endif