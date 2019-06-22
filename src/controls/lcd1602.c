#include "lcd1602.h"
#include "delay.h"
#include "libopencm3/cm3/common.h"

static struct {
    PIN_NAMES rs;
    PIN_NAMES en;
    PIN_NAMES db4;
    PIN_NAMES db5;
    PIN_NAMES db6;
    PIN_NAMES db7;
} lcd1602;

static void pulse(void) {
    gpio_pin_set(lcd1602.en, 1);
    delay_a_bit();
    gpio_pin_set(lcd1602.en, 0);
}

static void set_half_data(uint8_t d) {
    gpio_pin_set(lcd1602.db4, d & BIT0);
    gpio_pin_set(lcd1602.db5, d & BIT1);
    gpio_pin_set(lcd1602.db6, d & BIT2);
    gpio_pin_set(lcd1602.db7, d & BIT3);
}

static void send(uint8_t isData, uint8_t b) {
    gpio_pin_set(lcd1602.rs, isData);
    set_half_data(b >> 4);
    pulse();
    set_half_data(b);
    pulse();

    delay_some();
}

static void gpio_init(void) {
    gpio_pin_mode(lcd1602.rs, GPIO_OUTPUT);
    gpio_pin_mode(lcd1602.en, GPIO_OUTPUT);
    gpio_pin_mode(lcd1602.db4, GPIO_OUTPUT);
    gpio_pin_mode(lcd1602.db5, GPIO_OUTPUT);
    gpio_pin_mode(lcd1602.db6, GPIO_OUTPUT);
    gpio_pin_mode(lcd1602.db7, GPIO_OUTPUT);
}

void lcd1602_clear(void) {
    send(0, 0x01);
}

void lcd1602_put(char ch) {
    send(1, ch);
}

void lcd1602_put_str(char * str) {
    while(*str != 0) {
        lcd1602_put(*str);
        ++str;
    }
    
}

void lcd1602_pos(uint8_t row, uint8_t col) {
    switch (row)
    {
        case 0:
            send(0, (0x00+col) | 0x80);
            break;
        case 1:
            send(0, (0x40+col) | 0x80);
            break;
        case 2:
            send(0, (0x14+col) | 0x80);
            break;
        case 3:
            send(0, (0x54+col) | 0x80);
            break;
    }
}

void lcd1602_init(
    PIN_NAMES rs,
    PIN_NAMES en,
    PIN_NAMES db4,
    PIN_NAMES db5,
    PIN_NAMES db6,
    PIN_NAMES db7
) {
    lcd1602.rs = rs;
    lcd1602.en = en;
    lcd1602.db4 = db4;
    lcd1602.db5 = db5;
    lcd1602.db6 = db6;
    lcd1602.db7 = db7;

    gpio_init();

    send(0, 0x33);
    send(0, 0x32);

    send(0, 0x28);
    send(0, 0x0E);
    send(0, 0x01);
    send(0, 0x06);
    delay_some();

    send(0, 0x80);
}
