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

static void vSinTask(void * arg) {
    
}


int main(void) {
    rcc_init();

    RTOS_TASK(vSinTask, 0, RTOS_HIGH_PRIORITY);
    
    RTOS_START();

    while (1);
}
