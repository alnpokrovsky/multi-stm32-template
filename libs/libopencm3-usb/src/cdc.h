#ifndef __LIB_USB_PRIVATE_CDC_H__
#define __LIB_USB_PRIVATE_CDC_H__

#include <libopencm3/usb/usbd.h>

//  USB Endpoints.
#define MSC_OUT                 0x01
#define DATA_OUT                0x03

#define MSC_IN                  0x82
#define DATA_IN                 0x84
#define COMM_IN                 0x85

void cdc_setup(usbd_device* usbd_dev);

#endif  //  __LIB_USB_PRIVATE_CDC_H__
