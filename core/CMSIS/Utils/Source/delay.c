#include "delay.h"
#include <stdint.h>

extern uint32_t SystemCoreClock;

void delay_ms(int ms) {
	uint32_t ticks_in_ms = SystemCoreClock/1000;
	for (int i = 0; i < ms; i++) {
		for (uint32_t tick = 0; tick < ticks_in_ms; ++tick) {
			__asm__("nop");
		}
	}
}

/* redefine HAL function */
void HAL_Delay(uint32_t Delay);
void HAL_Delay(uint32_t Delay) {
	delay_ms(Delay);
}
