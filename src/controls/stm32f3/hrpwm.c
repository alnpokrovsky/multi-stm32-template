#if defined(STM32F3)

#include "hrpwm.h"
#include "hrtim.h"
#include "dma.h"
#include "sinarray.h"

#define PRESCAL         0
#define PERIOD          48000
#define DEAD_PRESCAL    5
#define DEAD_TIME       15

static const HRPWM_CHANNEL chs[] = {CH_A, CH_B};

void hrpwm_init(void) {
    hrtim_init_master(PRESCAL, PERIOD);

    hrtim_init_channel(chs[0], PRESCAL, PERIOD);
    hrtim_deadtime_channel(chs[0], DEAD_PRESCAL, DEAD_TIME, false, DEAD_TIME, false);
    //hrtim_duty_channel(chs[0], 24000);
    hrtim_dma_init_channel(chs[0], SIN_ARRAY);

    hrtim_init_channel(chs[1], PRESCAL, PERIOD);
    hrtim_deadtime_channel(chs[1], DEAD_PRESCAL, DEAD_TIME, false, DEAD_TIME, false);
    // hrtim_duty_channel(chs[1], 0);
    hrtim_dma_init_channel(chs[1], SIN_ARRAY);

    //hrtim_dma_init_burst_start(chs, 1, wave, 4);
    hrtim_start_channels(chs, 2);
    hrtim_dma_start_once_channel(chs[0], SIN_SIZE/2);
    //hrtim_dma_init_burst_start(&chs[0], 1, SIN_ARRAY, SIN_SIZE);
}

void dma_ch3_complete_handler(void) {
    hrtim_dma_start_once_channel(chs[1], SIN_SIZE/2);
}

void dma_ch4_complete_handler(void) {
    hrtim_dma_start_once_channel(chs[0], SIN_SIZE/2);
}

#endif