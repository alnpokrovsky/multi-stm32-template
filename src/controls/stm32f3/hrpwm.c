#if defined(STM32F3)

#include "hrpwm.h"
#include "stm32f3/hrtim.h"

uint16_t wave[] = {0x1027, 0x1027, 0x0080, 0x0080};

#define PRESCAL 3
#define PERIOD 48000

void hrpwm_init(void) {
    HRPWM_CHANNEL chs[] = {CH_A, CH_B};
    hrtim_init_master(PRESCAL, PERIOD);

    hrtim_init_channel(chs[0], PRESCAL, PERIOD);
    hrtim_deadtime_channel(chs[0], 3, 15, false, 15, false);
    hrtim_duty_channel(chs[0], 10000);
    //hrtim_dma_init_channel(CH_D, wave);

    hrtim_init_channel(chs[1], PRESCAL, PERIOD);
    hrtim_deadtime_channel(chs[1], 3, 15, false, 15, false);
    hrtim_duty_channel(chs[1], 10000);

    //hrtim_dma_init_burst_start(chs, 1, wave, 4);
    hrtim_start_channels(chs, 2);
}


#endif