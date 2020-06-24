#include "delay.h"
#include "rcc.h"


void delay_ms(int ms) {
	uint32_t ticks_in_ms = rcc_core_freq/1000;
	for (int i = 0; i < ms; i++) {
		for (uint32_t tick = 0; tick < ticks_in_ms; ++tick) {
			__asm__("nop");
		}
	}
}
