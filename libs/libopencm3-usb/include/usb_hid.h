#ifndef __LIB_USB_USB_HID_H__
#define __LIB_USB_USB_HID_H__

#include <stdint.h>
#include "usb_core.h"

void usb_hid_send(const uint8_t * data, uint16_t len);
void usb_hid_rx_handler(uint8_t * data, uint16_t len);


#endif