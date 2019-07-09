#ifndef __USB_LIB_H__
#define __USB_LIB_H__

#if defined(STM32F1) || defined(STM32F3) || defined(STM32F4)
#else
#error "USB lib: Unsupported device"
#endif

void usb_init(void);


#endif