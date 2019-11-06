#ifndef __HARDWARE_HRTIM_H__
#define __HARDWARE_HRTIM_H__

#include <stdint.h>
#include <stdbool.h>

#ifndef STM32F3
#error "not supported by other than stm32f3 series"
#endif

/**
 * A8  -> HRTIM1_CHA1, A9  -> HRTIM1_CHA2
 * A10 -> HRTIM1_CHB1, A11 -> HRTIM1_CHB2
 * B12 -> HRTIM1_CHC1, B13 -> HRTIM1_CHC2
 * B14 -> HRTIM1_CHD1, B15 -> HRTIM1_CHD2
 * C8  -> HRTIM1_CHE1,  C9 -> HRTIM1_CHE2 
 */
typedef enum {
    CH_A,
    CH_B,
    CH_C,
    CH_D,
    CH_E
} HRPWM_CHANNEL;

void hrtim_init_master(uint16_t prescaler, uint16_t period);

void hrtim_init_channel(HRPWM_CHANNEL ch, uint16_t prescaler, uint16_t period);

void hrtim_deadtime_channel(HRPWM_CHANNEL ch,
    uint16_t prescaler,
    uint16_t falling, bool overlapF,
    uint16_t rising,  bool overlapR
);

void hrtim_duty_channel(HRPWM_CHANNEL ch, uint16_t val);

void hrtim_start_channels(HRPWM_CHANNEL ch[], uint16_t n);

void hrtim_dma_init_channel(HRPWM_CHANNEL ch, uint16_t * waveform);

void hrtim_dma_start_once_channel(HRPWM_CHANNEL ch, uint16_t len);

void hrtim_dma_start_circular_channel(HRPWM_CHANNEL ch, uint16_t len);

void hrtim_dma_init_burst_start(
    HRPWM_CHANNEL chs[], uint16_t n,
    uint16_t * waveform, uint16_t len
);

#endif