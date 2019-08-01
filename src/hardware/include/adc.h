#ifndef __HARDWARE_ADC_H__
#define __HARDWARE_ADC_H__

#include <stdint.h>


typedef enum {
    ADC_1,
    ADC_2,
} ADC_NUM;


void adc_init(ADC_NUM adc);

uint16_t adc_read_blocking_channel(ADC_NUM adc, uint8_t channel);

#endif
