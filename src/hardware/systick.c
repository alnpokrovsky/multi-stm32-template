#include "systick.h"
#include <libopencm3/cm3/systick.h>

void systick_start_interrupt(uint32_t ticks) {
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(ticks);
	systick_interrupt_enable();
	systick_counter_enable();
}
