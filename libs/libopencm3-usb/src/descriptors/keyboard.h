#ifndef __LIB_USB_KEYBOARD_H__
#define __LIB_USB_KEYBOARD_H__

#include <libopencm3/usb/usbd.h>

void keyboard_setup(usbd_device * usbd_dev);

extern const struct usb_interface_descriptor keyboard_iface;

#endif