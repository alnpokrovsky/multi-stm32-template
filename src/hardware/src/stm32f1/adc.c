#if defined(STM32F1)

#include "adc.h"
#include "delay.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

typedef struct {
    uint32_t adc_base;
    uint16_t adc_rcc;
} adc_descript;

static const adc_descript ADCS[] = {
    {
        ADC1, RCC_ADC1,
    },
};

static void calibrate(ADC_NUM adc) {
    adc_reset_calibration(ADCS[adc].adc_base);
    adc_calibrate_async(ADCS[adc].adc_base);
    while (adc_is_calibrating(ADCS[adc].adc_base));
}

void adc_init(ADC_NUM adc) {
    rcc_periph_clock_enable(ADCS[adc].adc_rcc);
    adc_power_off(ADCS[adc].adc_base);

    rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV6);// Set. 12MHz
    adc_set_dual_mode(ADC_CR1_DUALMOD_IND); // Independent mode

    /* config for single conversion */
    adc_disable_scan_mode(ADCS[adc].adc_base);
    adc_set_single_conversion_mode(ADCS[adc].adc_base);
    adc_disable_external_trigger_regular(ADCS[adc].adc_base);
    
    adc_set_right_aligned(ADCS[adc].adc_base);
    
    adc_set_sample_time_on_all_channels(ADCS[adc].adc_base, ADC_SMPR_SMP_28DOT5CYC);

    adc_power_on(ADCS[adc].adc_base);

    calibrate(adc);
}

uint16_t adc_read_blocking_channel(ADC_NUM adc, uint8_t channel) {
    /* set one channel to read */
    adc_set_regular_sequence(ADCS[adc].adc_base, 1, &channel);
    /* start conversion without trigger */
    adc_start_conversion_direct(ADCS[adc].adc_base);

    /* wait for end of conversion */
    while (!adc_eoc(ADCS[adc].adc_base));
    return adc_read_regular(ADCS[adc].adc_base);
}

#endif
