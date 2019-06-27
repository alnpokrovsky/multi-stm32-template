#if defined(USE_MDR1986VE9x)

#include "systick.h"
#include <MDR32Fx.h>

static void null_handler(void)
{
	/* Do nothing. */
}

#pragma weak systick_handler = null_handler

void systick_start_interrupt(uint32_t ticks) {
    (void)ticks;

    NVIC_EnableIRQ(SysTick_IRQn);

    SysTick->LOAD = 0x9c40; //стартовое значение загружаемое в регистр VAL
    SysTick->VAL = 0;
    SysTick->CTRL =
                (1 << 0) | //включение таймера
                (1 << 1) | //разрешение прерывания
                (0 << 2); //источник синхросигнала = LSI
}

void SysTick_Handler(void);

void SysTick_Handler(void)
{
    systick_handler();
}

#endif