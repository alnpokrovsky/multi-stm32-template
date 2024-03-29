#if defined(STM32F1)

#include "../private/setup.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>

#define usb_driver st_usbfs_v1_usb_driver
#define NVIC_USB_RX NVIC_USB_LP_CAN_RX0_IRQ
#define USB_RX_HANDLER usb_lp_can_rx0_isr

static usbd_device * usbd_dev;

inline usbd_device * usb_setup(
    const struct usb_device_descriptor *dev,
    const struct usb_config_descriptor *conf,
    const char * const *strings, int num_strings,
    uint8_t *control_buffer, uint16_t control_buffer_size
) {
    rcc_periph_reset_pulse(RST_USB);

    usbd_dev = usbd_init(&usb_driver, 
        dev, conf, strings, num_strings, control_buffer, control_buffer_size);
    nvic_set_priority(NVIC_USB_RX, 0xFF);
    nvic_enable_irq(NVIC_USB_RX);
    return usbd_dev;
}

/**
 * This is the interrupt handler for low priority
 * USB events. Implementing a function with this 
 * name makes it the function used 
 * for the interrupt.
 */
void USB_RX_HANDLER(void) {
    usbd_poll(usbd_dev);
}

#endif