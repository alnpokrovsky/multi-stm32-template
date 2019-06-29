#ifndef __HARDWARE_TIM_H__
#define __HARDWARE_TIM_H__

#include <stdint.h>

typedef enum {
    TIM_1,
    TIM_2,
    TIM_3,
} TIM;

typedef enum {
    MILLISEC,
    MICROSEC,
} TIM_UNITS;

void tim_init(TIM tim, TIM_UNITS units);

void tim_start(TIM tim, uint16_t val);

void tim_stop(TIM tim);

void tim1_handler(void);

#endif