#ifndef __LIB_USB_DESCRIPTORS_CDC_H__
#define __LIB_USB_DESCRIPTORS_CDC_H__

#include "usbstd.h"


void cdc_setup(usbd_device* usbd_dev);

extern const struct usb_iface_assoc_descriptor cdc_iface_assoc;
extern const struct usb_interface_descriptor comm_iface;
extern const struct usb_interface_descriptor data_iface;

#endif  //  __LIB_USB_DESCRIPTORS_CDC_H__
