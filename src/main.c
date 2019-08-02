#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "encoder.h"
#include "adc.h"
#include "iic.h"
#include "controls/modbus.h"
#include "controls/pca9555.h"
#include "controls/pouconfig.h"
#include "delay.h"

#define BOOT1 PB_2


int main(void) {
    rcc_init();
    
    const POUCONFIG * config = pouconfig_init();
    
    digitalpin_mode(BOOT1, DIGITALPIN_INPUT);
    if (digitalpin_get(BOOT1)) {
        usb_init(config->usb_name);
        while (1);
    }

    encoder_init();
    adc_init(ADC_1);
    adc_init(ADC_2);

    iic_init(IIC_1);
    struct PCA9555* ioExpanders[config->ioCnt];
    for (int i = 0; i < config->ioCnt; ++i) {
        ioExpanders[i] = pca9555_init(&config->io[i]);
    }

    modbus_init(&config->modbus);
    
    while (1) {
        modbus_set_Ireg(0, encoder_get());
        
        modbus_set_Ireg(1, adc_read_blocking_channel(ADC_1, 1));
        modbus_set_Ireg(2, adc_read_blocking_channel(ADC_2, 2));

        for (int i = 0; i < config->ioCnt; ++i) {
            pca9555_write(ioExpanders[i], modbus_Coil_word(i));
            modbus_set_Coil_word(i, pca9555_read(ioExpanders[i]));
        }

        delay_a_bit();
        modbus_poll();
        modbus_poll();
    }
}
