#ifndef __CONTROLS_LCD_H__
#define __CONTROLS_LCD_H__

#include "digitalpin.h"

/** @example
 * lcd1602_init(PA_4, PA_5, PA_0, PA_1, PA_2, PA_3);
	lcd1602_clear();
	lcd1602_put('H');
	lcd1602_pos(1, 2);
	lcd1602_put('E');
 */

typedef struct {
    DIGITALPIN_NAME rs;
    DIGITALPIN_NAME en;
    DIGITALPIN_NAME db4;
    DIGITALPIN_NAME db5;
    DIGITALPIN_NAME db6;
    DIGITALPIN_NAME db7;
} LCD1602;

void lcd1602_init(LCD1602 * lcd);

void lcd1602_pos(LCD1602 * lcd, uint8_t row, uint8_t col);

void lcd1602_put(LCD1602 * lcd, char ch);

void lcd1602_put_str(LCD1602 * lcd, char * str);

void lcd1602_clear(LCD1602 * lcd);

#endif
