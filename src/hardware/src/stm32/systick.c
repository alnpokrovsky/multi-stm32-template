#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "systick.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

static void null_handler(void)
{
	/* Do nothing. */
}

#pragma weak systick_handler = null_handler

static const uint32_t PRESCAL_UNITS[] = {
    10000,    /* MILLISEC */
    1000000,  /* MICROSEC */
};

void systick_start_interrupt(uint32_t ticks, SYSTICK_UNITS units) {
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    uint32_t prescal = rcc_ahb_frequency / PRESCAL_UNITS[units];

	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(prescal * ticks);
	systick_interrupt_enable();
	/* start */
	systick_counter_enable();
}

void sys_tick_handler(void) {
	systick_handler();
}

#endif