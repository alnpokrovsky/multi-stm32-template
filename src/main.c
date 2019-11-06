#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "encoder.h"
#include "adc.h"
#include "iic.h"
#include "bootloader.h"
#include "controls/modbus.h"
#include "controls/pca9555.h"
#include "controls/pouconfig.h"
#include "controls/rtos.h"

#define BOOT1 PB_2

static void vModbusTask(void * arg) {
    POUCONFIG * config = arg;

    modbus_init(&config->modbus);
    
    while (1) {
        // digitalpin_toggle(PC_13);
        modbus_poll();
        vTaskDelay( 1 );
    }
}

static void vTwistersTask(void * arg) {
    (void) arg;
    encoder_init();
    adc_init(ADC_1);
    adc_init(ADC_2);

    while (1)
    {
        vTaskDelay( 2 );
        modbus_set_Ireg(0, encoder_get());
        vTaskDelay( 2 );        
        modbus_set_Ireg(1, adc_read_blocking_channel(ADC_1, 1));
        vTaskDelay( 2 );
        modbus_set_Ireg(2, adc_read_blocking_channel(ADC_2, 2));
    }
}

static void vExpandersTask(void * arg) {
    POUCONFIG * config = arg;

    iic_init(IIC_1);
    for (int i = 0; i < config->ioCnt; ++i) {
        pca9555_init(&config->io[i]);
    }

    while (1) {
        for (int i = 0; i < config->ioCnt; ++i) {
            vTaskDelay( 2 );

            pca9555_write(&config->io[i], modbus_Coil_word(i));
            modbus_set_Ists_word(i, pca9555_read(&config->io[i]));
        }
    }
}


int main(void) {
    rcc_init();
    
    POUCONFIG * config = pouconfig_init();

    // digitalpin_mode(PC_13, DIGITALPIN_OUTPUT);
    
    digitalpin_mode(BOOT1, DIGITALPIN_INPUT);
    if (digitalpin_get(BOOT1)) {
        usb_init();
        while (1);
    }

    RTOS_TASK(vModbusTask, config, RTOS_HIGH_PRIORITY);
    RTOS_TASK(vTwistersTask, config, RTOS_MEDIUM_PRIORITY);
    RTOS_TASK(vExpandersTask, config, RTOS_LOW_PRIORITY);

    RTOS_START();

    while (1);
}
