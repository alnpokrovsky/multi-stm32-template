#ifndef __UTILS_DELAY_H__
#define __UTILS_DELAY_H__

#include <stdint.h>

/**
 * Initialize the timers used for delays.
 */
void delay_setup(void);

/**
 * busy wait for a number of usecs.
 * @param us number of usecs to delay.
 */
void delay_us(uint16_t us);

void delay_some(void);

void delay_a_bit(void);

#endif
