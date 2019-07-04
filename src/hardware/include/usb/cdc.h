#ifndef __HARDWARE_USB_CDC_H__
#define __HARDWARE_USB_CDC_H__

void usb_cdc_init(void);

void usb_cdc_send(const char * buf, int len);

void usb_cdc_read_handler(char * buf, int len);

#endif
