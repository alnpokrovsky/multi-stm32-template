#ifndef __HARDWARE_ENCODER_H__
#define __HARDWARE_ENCODER_H__

#include <stdint.h>

/**
 * init PA6 and PA7 in timer3 encoder mode
 */
void encoder_init(void);

/**
 * get timer3 value (encoder ticks)
 */
uint16_t encoder_get(void);

#endif
