#include "adc.h"
#include "delay.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#if defined(STM32F1)

static void calibrate(void) {
    delay_some();
    adc_reset_calibration(ADC1);
    adc_calibrate(ADC1);
}

void adc_init(void) {
    rcc_periph_clock_enable(RCC_ADC1);
    adc_power_off(ADC1);

    /* config for single conversion */
    adc_disable_scan_mode(ADC1);
    adc_set_single_conversion_mode(ADC1);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_right_aligned(ADC1);
    
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

    adc_power_on(ADC1);

    calibrate();

    uint8_t channel_seq[] = {1, 2};
    adc_set_regular_sequence(ADC1, sizeof(channel_seq), channel_seq);
}

uint16_t adc_read(void) {
    /* start conversion without trigger */
    adc_start_conversion_direct(ADC1);

    /* wait for end of conversion */
    while (!adc_eoc(ADC1));
    return adc_read_regular(ADC1);
}

#endif
