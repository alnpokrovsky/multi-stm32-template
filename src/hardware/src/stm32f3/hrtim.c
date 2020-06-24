#if defined(STM32F3)

#include "hrtim.h"
#include "dma.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/hrtim.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#define RCC_CFGR3_HRTIM1SW (1 << 12)

#define DTPRSC(x)       ( ((x) << HRTIM_TIMx_DT_DTPRSC_SHIFT) & HRTIM_TIMx_DT_DTPRSC_MASK )
#define DTRx(x)         ( ((x) << HRTIM_TIMx_DT_DTRx_SHIFT)   & HRTIM_TIMx_DT_DTRx_MASK   )
#define DTFx(x)         ( ((x) << HRTIM_TIMx_DT_DTFx_SHIFT)   & HRTIM_TIMx_DT_DTFx_MASK   )
#define MCR_CK_PSC(x)   ( ((x) << HRTIM_MCR_CK_PSC_SHIFT)     & HRTIM_MCR_CK_PSC_MASK     )
#define TIMx_CK_PSC(x)  ( ((x) << HRTIM_TIMx_CR_CK_PSCx_SHIFT)& HRTIM_TIMx_CR_CK_PSCx_MASK)

typedef struct {
    uint32_t tim_base;
    uint32_t tim_outputs_en;
    uint32_t tim_en;
    DMA_CHANNEL dma;
    uint32_t gpio_port;
    uint16_t gpio_port_rcc;
    uint32_t gpio_pins;     //< CH1, CH2, FLT 
    uint8_t gpio_pins_af;
} Chanel_descript;


static const Chanel_descript CHANNELS[] = {
    { /* Channel A */
        HRTIM_TIMA, HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN, HRTIM_MCR_TACEN,
        DMA_CH3,
        GPIOA, RCC_GPIOA, GPIO8|GPIO9|GPIO12, GPIO_AF13
    },
    { /* Channel B */
        HRTIM_TIMB, HRTIM_OENR_TB1OEN | HRTIM_OENR_TB2OEN, HRTIM_MCR_TBCEN,
        DMA_CH4,
        GPIOA, RCC_GPIOA, GPIO10|GPIO11|GPIO15, GPIO_AF13
    },
    { /* Channel C */
        HRTIM_TIMC, HRTIM_OENR_TC1OEN | HRTIM_OENR_TC2OEN, HRTIM_MCR_TCCEN,
        DMA_CH5,
        GPIOB, RCC_GPIOB, GPIO12|GPIO13|GPIO10, GPIO_AF13
    },
    { /* Channel D */
        HRTIM_TIMD, HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN, HRTIM_MCR_TDCEN,
        DMA_CH6,
        GPIOB, RCC_GPIOB, GPIO14|GPIO15|GPIO11, GPIO_AF13
    },
    { /* Channel E */
        HRTIM_TIME, HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN, HRTIM_MCR_TECEN,
        DMA_CH7,
        GPIOC, RCC_GPIOC, GPIO8|GPIO9|GPIO7, GPIO_AF3
    },
};

static void init_gpio(HRPWM_CHANNEL ch) {
    rcc_periph_clock_enable(CHANNELS[ch].gpio_port_rcc);
    gpio_mode_setup(CHANNELS[ch].gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, CHANNELS[ch].gpio_pins);
    gpio_set_output_options(CHANNELS[ch].gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, CHANNELS[ch].gpio_pins);    
    gpio_set_af(CHANNELS[ch].gpio_port, CHANNELS[ch].gpio_pins_af, CHANNELS[ch].gpio_pins);
}

static void sync_gpio_init(void) {
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1|GPIO2);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO1|GPIO2);    
    gpio_set_af(GPIOB, GPIO_AF13, GPIO1|GPIO2);
}

void hrtim_init_master(uint16_t prescaler, uint16_t period) {
    rcc_periph_clock_enable(RCC_HRTIM);
    /* switch APB to PLLx2 rcc source */
    RCC_CFGR3 |= RCC_CFGR3_HRTIM1SW;

    /* calibration master hrtim */
    HRTIM_DLLCR |= HRTIM_DLLCR_CAL | HRTIM_DLLCR_CALEN;
    /* wait until ready */
    while((HRTIM_ISR & HRTIM_ISR_DLLRDY) == 0);

    /* Master counter in continuous mode */
    HRTIM_MCR |= HRTIM_MCR_CONT | HRTIM_MCR_RETRIG;

    /* sync output settings */
    HRTIM_MCR |= HRTIM_MCR_SYNC_SRC_MSTART; // master reset src
    HRTIM_MCR |= HRTIM_MCR_SYNC_OUT_NEG; // output generates pos edge
    /* sync input settings */
    //HRTIM_MCR |= HRTIM_MCR_SYNC_IN_MASK; // sync_input pos edge src
    //HRTIM_MCR |= HRTIM_MCR_SYNCRSTM; // start or resets timer
    /* sync gpio settings */
    sync_gpio_init();

    /* master tim period */
    HRTIM_MCR |= MCR_CK_PSC(prescaler);
    HRTIM_MPER = period;
    /* half mode: cmp1 = HRTIM_MPER/2 */
    HRTIM_MCR |= HRTIM_MCR_HALF;
    
    /* Preload enabled on REP event */
    HRTIM_MCR |= HRTIM_MCR_PREEN | HRTIM_MCR_MREPU;
}

void hrtim_init_channel(HRPWM_CHANNEL ch, uint16_t prescaler, uint16_t period) {
    init_gpio(ch);

    HRTIM_TIMx_TIMCR(CHANNELS[ch].tim_base) |= TIMx_CK_PSC(prescaler);
    HRTIM_TIMx_PER(CHANNELS[ch].tim_base) = period;
    HRTIM_TIMx_CMP1(CHANNELS[ch].tim_base) = 0;
    HRTIM_TIMx_CMP2(CHANNELS[ch].tim_base) = 0;

    /* set 101 */
    HRTIM_TIMx_SET1(CHANNELS[ch].tim_base) |= HRTIM_TIMx_SETy_CMP1;
    HRTIM_TIMx_RST1(CHANNELS[ch].tim_base) |= HRTIM_TIMx_RSTy_CMP2 | HRTIM_TIMx_RSTy_PER;

    /* tim in continuous mode */
    HRTIM_TIMx_TIMCR(CHANNELS[ch].tim_base) |= HRTIM_TIMx_CR_CONT | HRTIM_TIMx_CR_RETRIG;
    /* tim reset on sync */
    //HRTIM_TIMx_TIMCR(CHANNELS[ch].tim_base) |= HRTIM_TIMx_CR_SYNCRSTx;

    /* config complementary channels */
    HRTIM_OENR |= CHANNELS[ch].tim_outputs_en;

    /* tim preload enabled on REP event */
    HRTIM_TIMx_TIMCR(CHANNELS[ch].tim_base) |= HRTIM_TIMx_CR_PREEN | HRTIM_TIMx_CR_TxREPU;
}

void hrtim_deadtime_channel(
    HRPWM_CHANNEL ch,
    uint16_t prescaler,
    uint16_t falling,
    bool overlapF,
    uint16_t rising,
    bool overlapR
) {
    HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= DTPRSC(prescaler); // set prescaler

    HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= DTFx(falling); // faling value
    if (overlapF) HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= HRTIM_TIMx_DT_SDTFx; // overlap falling
    HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= HRTIM_TIMx_DT_DTFLKx; // lock falling value

    HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= DTRx(rising); // rising value
    if (overlapR) HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= HRTIM_TIMx_DT_SDTRx; // overlap rising
    HRTIM_TIMx_DT(CHANNELS[ch].tim_base) |= HRTIM_TIMx_DT_DTRLKx; // lock rising value

    HRTIM_TIMx_OUT(CHANNELS[ch].tim_base) |= HRTIM_TIMx_OUT_DTEN; // enable deadtime
}

void hrtim_duty_channel(HRPWM_CHANNEL ch, uint16_t val) {
    HRTIM_TIMx_CMP1(CHANNELS[ch].tim_base) = val;
}

inline void hrtim_start_channels(const HRPWM_CHANNEL chs[], uint16_t n) {
    uint32_t en = HRTIM_MCR_MCEN;
    for (int i = 0; i < n; i++) {
        en |= CHANNELS[chs[i]].tim_en;
    }
    /* timer counters enable at same time */
    HRTIM_MCR |= en;
}

void hrtim_dma_init_channel(HRPWM_CHANNEL ch, const uint16_t * waveform) {
	dma_mem2periph_init(CHANNELS[ch].dma, 
        (uint32_t) waveform, 
        (uint32_t) &HRTIM_TIMx_CMP1(CHANNELS[ch].tim_base) );
    /* enable DMA for tim on repetition event */
    HRTIM_TIMx_DIER(CHANNELS[ch].tim_base) |= HRTIM_TIMx_DIER_REPDE;
}

void hrtim_dma_start_once_channel(HRPWM_CHANNEL ch, uint16_t len) {
    dma_start_once(CHANNELS[ch].dma, len);
}

void hrtim_dma_start_cyclic_channel(HRPWM_CHANNEL ch, uint16_t len) {
    dma_start_cyclic(CHANNELS[ch].dma, len);
}

void hrtim_dma_init_burst_start(
    HRPWM_CHANNEL chs[], uint16_t n,
    const uint16_t * waveform, uint16_t len
) {
    /* special master tim DMA channel */
    dma_mem2periph_init(DMA_CH2, 
        (uint32_t) waveform, 
        (uint32_t) &HRTIM_BDMADR );

    /* clock sourse */
    HRTIM_BMCR |= HRTIM_BMCR_BMCLK_MASTER; //master
    HRTIM_BMPER = 1; // period 1
    HRTIM_BMCMPR6 = 1;

    /* Burst Mode Preload Enable */
    HRTIM_BMCR |= HRTIM_BMCR_BMPREN;
    /* Burst Mode Continuous */
    HRTIM_BMCR |= HRTIM_BMCR_BMOM;

    /* Update done on master timer roll-over following a DMA burst transfer completion */
    HRTIM_MCR |= HRTIM_MCR_BRSTDMA_COMPL;

    for (int i = 0; i < n; i++) {
        /* the update occurs on the update event following the DMA burst transfer completion */
        HRTIM_TIMx_TIMCR(CHANNELS[chs[i]].tim_base) |= HRTIM_TIMx_CR_UPDGAT_DMA_POST;
        /* write to register cmp1 for tim ch */
        HRTIM_BDTxUPR(CHANNELS[chs[i]].tim_base) |= HRTIM_BDTxUPR_TIMxCMP1 | HRTIM_BDTxUPR_TIMxCMP2;
    }
    
    /* dma on master repetition */
    HRTIM_MDIER |= HRTIM_MDIER_MCMP1DE;
    /* Burst Mode Enable */
    HRTIM_BMCR |= HRTIM_BMCR_BME;

    dma_start_cyclic(DMA_CH2, len);
}

void hrtim_flt_isr(void) {

}

#endif