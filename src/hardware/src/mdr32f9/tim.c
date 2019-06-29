#if defined(USE_MDR1986VE9x)

#include "tim.h"
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>

static void null_handler(void) {}

#pragma weak tim1_handler = null_handler

typedef struct {
    MDR_TIMER_TypeDef * tim_base;
    uint8_t tim_irq;
} tim_descript;

static const tim_descript TIMS[] = {
    {
        MDR_TIMER1, TIMER1_IRQn,
    },
};

static const uint32_t PRESCAL_UNITS[] = {
    8000,     /* MILLISEC * div8 */
    8000000,  /* MICROSEC * div8 */
};

void tim_init(TIM tim, TIM_UNITS units) {
    /* Включение тактирования таймера */
    RST_CLK_PCLKcmd(PCLK_BIT(TIMS[tim].tim_base), ENABLE);
    /* Установка предделителя тактовой частоты таймера */
    TIMER_BRGInit(TIMS[tim].tim_base, TIMER_HCLKdiv8);

    /* Count tim timeout units */
    RST_CLK_FreqTypeDef RST_CLK_Clocks;
	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);
    uint32_t cpuclock = RST_CLK_Clocks.CPU_CLK_Frequency;
    uint32_t prescal = cpuclock / PRESCAL_UNITS[units];
    
    /* tim base config */
    TIMER_CntInitTypeDef TIMInit;
    TIMER_CntStructInit(&TIMInit);
    /* делитель частоты */
    TIMInit.TIMER_Prescaler = prescal - 1;
    TIMER_CntInit(TIMS[tim].tim_base, &TIMInit);

    //Настройка прерывания
    NVIC_EnableIRQ(TIMS[tim].tim_irq);
    NVIC_SetPriority(TIMS[tim].tim_irq, 0);
}

void tim_start(TIM tim, uint16_t val) {
    /* основание счета */
    TIMS[tim].tim_base->ARR = val;
    /* ждем, пока не выставится время */
    while (TIMER_GetCntWriteComplete(TIMS[tim].tim_base) == SET) ;

    /* Включение прерывания при равенстве нулю */
    TIMER_ITConfig(TIMS[tim].tim_base, TIMER_STATUS_CNT_ARR, ENABLE);
    /* Запуск таймера */
    TIMER_Cmd(TIMS[tim].tim_base, ENABLE);
}

void tim_stop(TIM tim) {
    TIMER_ITConfig(TIMS[tim].tim_base, TIMER_STATUS_CNT_ARR, DISABLE);
    TIMER_Cmd(TIMS[tim].tim_base, DISABLE);
}


void Timer1_IRQHandler(void);
void Timer1_IRQHandler(void) {
    if(TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ARR))
    {
        tim1_handler();
        // Очистка флага прерывания в таймере (предотвращает повторный вызов того же прерывания)
        TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
    }
}

#endif