#ifndef __HARDWARE_USB_HID_H__
#define __HARDWARE_USB_HID_H__

#include <stdint.h>

void usb_hid_init(void);

void usb_hid_send(const uint8_t * buf, uint16_t len);

void usb_hid_read_handler(uint8_t * buf, uint16_t len);

#endif