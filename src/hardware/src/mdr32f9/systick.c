#ifndef FREERTOS_LIB
#if defined(USE_MDR1986VE9x)

#include "systick.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_it.h>

__attribute__((weak))
void systick_handler(void)
{
	/* Do nothing. */
}


void systick_start_interrupt(uint32_t ticks, SYSTICK_UNITS units) {
    (void)ticks;
    (void)units;

    NVIC_EnableIRQ(SysTick_IRQn);

    SysTick->LOAD = 0x9c40; //стартовое значение загружаемое в регистр VAL
    SysTick->VAL = 0;
    SysTick->CTRL =
                (1 << 0) | //включение таймера
                (1 << 1) | //разрешение прерывания
                (0 << 2); //источник синхросигнала = LSI
}

void SysTick_Handler(void)
{
    systick_handler();
}

#endif
#endif
