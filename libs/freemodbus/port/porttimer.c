/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- libopencm3 STM32F includes -------------------------------*/
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* Enable TIM2 clock. */
    rcc_periph_clock_enable(RCC_TIM2);
    /* Enable TIM2 interrupt. */
 	nvic_enable_irq(NVIC_TIM2_IRQ);
    /* Reset TIM2 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM2);
/* Timer global mode: - Divider 4, Alignment edge, Direction up */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_continuous_mode(TIM2);
	timer_set_prescaler(TIM2, ((2*rcc_apb1_frequency)/20000ul - 1)); /* 50 microseconds period */
    timer_set_period(TIM2, usTim1Timerout50us);
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    TIM2_CNT = 1;
	timer_enable_irq(TIM2, TIM_DIER_UIE);
    timer_enable_counter(TIM2);
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
    timer_disable_irq(TIM2, TIM_DIER_UIE);
    timer_disable_counter(TIM2);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static CHAR count;
void tim2_isr(void)
{
    count++;
	if (timer_interrupt_source(TIM2, TIM_SR_UIF)) {
        timer_clear_flag(TIM2, TIM_SR_UIF); /* Clear interrrupt flag. */
    }
	timer_get_flag(TIM2, TIM_SR_UIF);	/* Reread to force the previous (buffered) write before leaving */
    pxMBPortCBTimerExpired();
}

