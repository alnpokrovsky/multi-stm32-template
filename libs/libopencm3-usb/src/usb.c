#include "usb.h"
#include "descriptors/usb_core.h"
#include <libopencm3/stm32/desig.h>
#include "UsbConfig.h"
#include "delay.h"


void usb_init(void) {
    char serial[USB_SERIAL_NUM_LENGTH+1];
    desig_get_unique_id_as_string(serial, USB_SERIAL_NUM_LENGTH+1);
    usb_core_set_serial_number(serial);

    usb_core_init();

    delay_some();
}
