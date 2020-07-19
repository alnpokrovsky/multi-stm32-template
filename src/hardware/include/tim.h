#ifndef __HARDWARE_TIM_H__
#define __HARDWARE_TIM_H__

#include <stdint.h>

typedef enum {
    TIM_2,
    TIM_3,
    TIM_4,
} TIM;

typedef enum {
    MILLISEC,
    MICROSEC,
} TIM_UNITS;

void tim_init(TIM tim, uint16_t val, TIM_UNITS units);

void tim_start_once(TIM tim);

void tim_start_cyclic(TIM tim);

void tim_stop(TIM tim);

void tim2_handler(void);
void tim3_handler(void);
void tim4_handler(void);


#endif