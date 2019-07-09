#ifndef __HARDWARE_USB_H__
#define __HARDWARE_USB_H__

#include <libopencm3/usb/usbd.h>

usbd_device * usb_setup(
    const struct usb_device_descriptor *dev,
    const struct usb_config_descriptor *conf,
    const char * const *strings, int num_strings,
    uint8_t *control_buffer, uint16_t control_buffer_size
);


#endif
