#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "tim.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>


static void null_handler(void) {}

#pragma weak tim2_handler = null_handler
#pragma weak tim3_handler = null_handler
#pragma weak tim4_handler = null_handler


typedef struct {
    uint32_t tim_base;
    uint16_t tim_rcc;
    uint32_t tim_rst;
    uint8_t tim_irq;
} tim_descript;

static const tim_descript TIMS[] = {
    {
        TIM2, RCC_TIM2, RST_TIM2, NVIC_TIM2_IRQ,
    },
    {
        TIM3, RCC_TIM3, RST_TIM3, NVIC_TIM3_IRQ,
    },
    {
        TIM4, RCC_TIM4, RST_TIM4, NVIC_TIM4_IRQ,
    },
};

static const uint32_t PRESCAL_UNITS[] = {
    2000,     /* MILLISEC */
    2000000,  /* MICROSEC */
};


void tim_init(TIM tim, uint16_t val, TIM_UNITS units) {
    /* Enable rcc */
    rcc_periph_clock_enable(TIMS[tim].tim_rcc);
    
    /* Reset TIM peripheral to defaults. */
	rcc_periph_reset_pulse(TIMS[tim].tim_rst);

    /* Timer global mode:
     * Divider 1, 
     * Alignment edge, 
     * Direction up
     */
	timer_set_mode(TIMS[tim].tim_base, 
        TIM_CR1_CKD_CK_INT, 
        TIM_CR1_CMS_EDGE, 
        TIM_CR1_DIR_UP
    );

    uint32_t prescal = 2*rcc_apb1_frequency / PRESCAL_UNITS[units];

	timer_set_prescaler(TIMS[tim].tim_base, prescal - 1);
    timer_set_period(TIMS[tim].tim_base, val - 1);

    /* Enable NVIC TIM interrupt. */    
    nvic_set_priority(TIMS[tim].tim_irq, NVIC_HIGH);
 	nvic_enable_irq(TIMS[tim].tim_irq);
}

static void tim_start(TIM tim) {
    TIM_CNT(TIMS[tim].tim_base) = 1;
    timer_enable_irq(TIMS[tim].tim_base, TIM_DIER_UIE);
    timer_enable_counter(TIMS[tim].tim_base);
}

void tim_start_once(TIM tim) {
    timer_one_shot_mode(TIMS[tim].tim_base);
    tim_start(tim);
}

void tim_start_cyclic(TIM tim) {
	timer_continuous_mode(TIMS[tim].tim_base);
    tim_start(tim);
}

void tim_stop(TIM tim) {
    /* Disable any pending timers. */
    timer_disable_irq(TIMS[tim].tim_base, TIM_DIER_UIE);
    timer_disable_counter(TIMS[tim].tim_base);
}



void tim2_isr(void)
{
	if (timer_interrupt_source(TIM2, TIM_SR_UIF)) {
        tim2_handler();
        /* Clear interrupt flag. */
        timer_clear_flag(TIM2, TIM_SR_UIF);
    }
}

void tim3_isr(void)
{
	if (timer_interrupt_source(TIM3, TIM_SR_UIF)) {
        tim3_handler();
        /* Clear interrupt flag. */
        timer_clear_flag(TIM3, TIM_SR_UIF); 
    }
}

void tim4_isr(void)
{
	if (timer_interrupt_source(TIM4, TIM_SR_UIF)) {
        tim4_handler();
        /* Clear interrupt flag. */
        timer_clear_flag(TIM4, TIM_SR_UIF);
    }
}

#endif