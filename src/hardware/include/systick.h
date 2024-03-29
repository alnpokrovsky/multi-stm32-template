#ifndef FREERTOS_LIB


#ifndef __HARDWARE_SYSTICK_H__
#define __HARDWARE_SYSTICK_H__


#include <stdint.h>

typedef enum {
    MILLISEC,
    MICROSEC,
} SYSTICK_UNITS;

/**
 * init systick timer and start it
 * release 'void sys_tick_handler(void)' from nvic.h
 * to enter interrupt
 */
void systick_start_interrupt(uint32_t ticks, SYSTICK_UNITS units);

void systick_handler(void);


#endif

#endif
