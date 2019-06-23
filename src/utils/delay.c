#include <stdint.h>

#include "delay.h"

void delay_some() {
	for (int i = 0; i < 800000; i++) {
		__asm__("nop");
	}
}

void delay_a_bit() {
	for (int i = 0; i < 10000; i++) {
		__asm__("nop");
	}
}
