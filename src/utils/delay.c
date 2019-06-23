#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "delay.h"

void delay_setup(void)
{
	/* set up a microsecond free running timer for ... things... */
	rcc_periph_clock_enable(RCC_TIM4);
	/* microsecond counter */
	timer_set_prescaler(TIM4, rcc_apb1_frequency / 1e6 - 1);
	timer_set_period(TIM4, 0xffff);
	timer_one_shot_mode(TIM4);
}

void delay_us(uint16_t us)
{
	TIM_ARR(TIM4) = us;
	TIM_EGR(TIM4) = TIM_EGR_UG;
	TIM_CR1(TIM4) |= TIM_CR1_CEN;
	//timer_enable_counter(TIM4);
	while (TIM_CR1(TIM4) & TIM_CR1_CEN);
}

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
