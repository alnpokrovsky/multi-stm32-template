#include "lcd1602.h"
#include "delay.h"

static void delay_a_bit(void) {
	for (int i = 0; i < 10000; i++) {
		__asm__("nop");
	}
}

static void pulse(LCD1602 * lcd) {
    gpio_set(&lcd->en, GPIO_PINS_ON);
    delay_a_bit();
    gpio_set(&lcd->en, GPIO_PINS_OFF);
}

static void set_half_data(LCD1602 * lcd, uint8_t d) {
    gpio_set(&lcd->db4, (d & 0x01) ? GPIO_PINS_ON : GPIO_PINS_OFF );
    gpio_set(&lcd->db5, (d & 0x02) ? GPIO_PINS_ON : GPIO_PINS_OFF );
    gpio_set(&lcd->db6, (d & 0x04) ? GPIO_PINS_ON : GPIO_PINS_OFF );
    gpio_set(&lcd->db7, (d & 0x08) ? GPIO_PINS_ON : GPIO_PINS_OFF );
}

static void send(LCD1602 * lcd, uint8_t isData, uint8_t b) {
    gpio_set(&lcd->rs, isData);
    set_half_data(lcd, b >> 4);
    pulse(lcd);
    set_half_data(lcd, b);
    pulse(lcd);

    delay_ms(10);
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
    gpio_init(&lcd->rs);
    gpio_init(&lcd->en);
    gpio_init(&lcd->db4);
    gpio_init(&lcd->db5);
    gpio_init(&lcd->db6);
    gpio_init(&lcd->db7);

    send(lcd, 0, 0x33);
    send(lcd, 0, 0x32);

    send(lcd, 0, 0x28);
    send(lcd, 0, 0x0E);
    send(lcd, 0, 0x01);
    send(lcd, 0, 0x06);
    delay_ms(10);

    send(lcd, 0, 0x80);
}
