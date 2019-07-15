#ifndef __LIB_USB_PRIVATE_CORE_AGGREGATE_H__
#define __LIB_USB_PRIVATE_CORE_AGGREGATE_H__

#include <libopencm3/usb/usbd.h>


int aggregate_register_config_callback(
    usbd_device *usbd_dev,
	usbd_set_config_callback callback
);

int aggregate_register_callback(
    usbd_device *usbd_dev, 
    uint8_t type,
    uint8_t type_mask,
    usbd_control_callback callback
);

enum usbd_request_return_codes aggregate_callback(
    usbd_device *usbd_dev,
	struct usb_setup_data *req, 
    uint8_t **buf, 
    uint16_t *len,
	usbd_control_complete_callback *complete
);

void aggregate_set_callback (
    usbd_device *usbd_dev,
    uint16_t wValue
);

#endif