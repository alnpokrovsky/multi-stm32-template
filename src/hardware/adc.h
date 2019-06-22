#ifndef __HARDWARE_ADC_H__
#define __HARDWARE_ADC_H__

#include <stdint.h>

void adc_init(void);

uint16_t adc_read(void);

#endif
