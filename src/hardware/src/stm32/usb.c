#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#ifndef OCM3_USB_LIB
#error "You did not include USB library"
#endif

#include "usb.h"
#include "usb_core.h"
#include <libopencm3/stm32/desig.h>
#include "UsbConfig.h"
#include "delay.h"
#include <stdlib.h>


void usb_init(const char* name) {
    if (name != NULL) {
        usb_core_set_serial_number(name);
    } else {
        char serialN[USB_SERIAL_NUM_LENGTH+1];
        desig_get_unique_id_as_string(serialN, USB_SERIAL_NUM_LENGTH+1);
        usb_core_set_serial_number(serialN);
    }

    usb_core_init();

    delay_some();
}

#endif