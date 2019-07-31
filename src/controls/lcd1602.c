#include "lcd1602.h"
#include "delay.h"


static void pulse(LCD1602 * lcd) {
    digitalpin_set(lcd->en, 1);
    delay_a_bit();
    digitalpin_set(lcd->en, 0);
}

static void set_half_data(LCD1602 * lcd, uint8_t d) {
    digitalpin_set(lcd->db4, d & 0x01);
    digitalpin_set(lcd->db5, d & 0x02);
    digitalpin_set(lcd->db6, d & 0x04);
    digitalpin_set(lcd->db7, d & 0x08);
}

static void send(LCD1602 * lcd, uint8_t isData, uint8_t b) {
    digitalpin_set(lcd->rs, isData);
    set_half_data(lcd, b >> 4);
    pulse(lcd);
    set_half_data(lcd, b);
    pulse(lcd);

    delay_some();
}

static void gpio_init(LCD1602 * lcd) {
    digitalpin_mode(lcd->rs, DIGITALPIN_OUTPUT);
    digitalpin_mode(lcd->en, DIGITALPIN_OUTPUT);
    digitalpin_mode(lcd->db4, DIGITALPIN_OUTPUT);
    digitalpin_mode(lcd->db5, DIGITALPIN_OUTPUT);
    digitalpin_mode(lcd->db6, DIGITALPIN_OUTPUT);
    digitalpin_mode(lcd->db7, DIGITALPIN_OUTPUT);
}

void lcd1602_clear(LCD1602 * lcd) {
    send(lcd, 0, 0x01);
}

void lcd1602_put(LCD1602 * lcd, char ch) {
    send(lcd, 1, ch);
}

void lcd1602_put_str(LCD1602 * lcd, char * str) {
    while(*str != 0) {
        lcd1602_put(lcd, *str);
        ++str;
    }
    
}

void lcd1602_pos(LCD1602 * lcd, uint8_t row, uint8_t col) {
    switch (row)
    {
        case 0:
            send(lcd, 0, (0x00+col) | 0x80);
            break;
        case 1:
            send(lcd, 0, (0x40+col) | 0x80);
            break;
        case 2:
            send(lcd, 0, (0x14+col) | 0x80);
            break;
        case 3:
            send(lcd, 0, (0x54+col) | 0x80);
            break;
    }
}

void lcd1602_init(LCD1602 * lcd) {
    gpio_init(lcd);

    send(lcd, 0, 0x33);
    send(lcd, 0, 0x32);

    send(lcd, 0, 0x28);
    send(lcd, 0, 0x0E);
    send(lcd, 0, 0x01);
    send(lcd, 0, 0x06);
    delay_some();

    send(lcd, 0, 0x80);
}
