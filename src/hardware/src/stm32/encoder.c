#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "encoder.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#define SHIFT 2
#define PERIOD 1024

void encoder_init(void) {
    rcc_periph_clock_enable(RCC_TIM3);
	timer_set_period(TIM3, PERIOD << SHIFT);
	timer_slave_set_mode(TIM3, 0x3); // encoder
	timer_ic_set_input(TIM3, TIM_IC1, TIM_IC_IN_TI1);
	timer_ic_set_input(TIM3, TIM_IC2, TIM_IC_IN_TI2);
	timer_enable_counter(TIM3);
}

uint16_t encoder_get(void) {
    return (uint16_t) timer_get_counter(TIM3) >> SHIFT;
}

#endif