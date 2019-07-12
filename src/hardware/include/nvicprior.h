#ifndef __HARDWARE_NVIC_PRIOR_H__
#define __HARDWARE_NVIC_PRIOR_H__

typedef enum {
    NVIC_HIGHEST = 0x00,
    NVIC_HIGH    = 0x0F,
    NVIC_MEDIUM  = 0x4F,
    NVIC_LOW     = 0x8F,
    NVIC_LOWEST  = 0xFF,

} NVIC_PRIOR;

#endif