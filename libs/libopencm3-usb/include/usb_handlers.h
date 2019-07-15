#ifndef __LIB_USB_USB_HANDLERS_H__
#define __LIB_USB_USB_HANDLERS_H__

#include <stdint.h>

void usb_cdc_rx_handler(char * data, uint16_t len);

#endif