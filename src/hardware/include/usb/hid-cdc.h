#ifndef __HARDWARE_USB_HID_CDC_H__
#define __HARDWARE_USB_HID_CDC_H__

void hid_cdc_init(void);

void hid_cdc_send(const char * buf, int len);

void hid_cdc_read_handler(char * buf, int len);

#endif
