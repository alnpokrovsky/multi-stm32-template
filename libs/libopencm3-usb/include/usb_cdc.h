#ifndef __LIB_USB_CDC_H__
#define __LIB_USB_CDC_H__

#include <stdint.h>
#include "usb_core.h"

uint16_t usb_cdc_tx(const char * data, uint16_t len);
void usb_cdc_rx_handler(char * data, uint16_t len);

#endif