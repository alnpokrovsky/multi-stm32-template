#ifndef __USB_LIB_DESCRIPTORS_HID_H__
#define __USB_LIB_DESCRIPTORS_HID_H__

#include <libopencm3/usb/usbd.h>

void hid_setup(usbd_device *usbd_dev);

extern const struct usb_interface_descriptor hid_iface;

#endif