#ifndef __LIB_USB_PRIVATE_MSC_H__
#define __LIB_USB_PRIVATE_MSC_H__

#include <stdint.h>
#include <libopencm3/usb/usbd.h>


void msc_setup(usbd_device* usbd_dev0);

extern const struct usb_interface_descriptor msc_iface;

#endif