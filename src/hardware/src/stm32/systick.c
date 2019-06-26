#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "systick.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

static void null_handler(void)
{
	/* Do nothing. */
}

#pragma weak systick_handler = null_handler

void systick_start_interrupt(uint32_t ticks) {
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(ticks);
	systick_interrupt_enable();
	/* start */
	systick_counter_enable();
}

void sys_tick_handler(void) {
	systick_handler();
}

#endif