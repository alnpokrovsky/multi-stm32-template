/* only valid for disco-f429zi hs usb config */

#if defined(STM32F4)

#include "../private/setup.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define usb_driver otghs_usb_driver
#define RCC_USB RCC_OTGHS
#define NVIC_USB_RX NVIC_OTG_HS_IRQ
#define USB_RX_HANDLER otg_hs_isr


static usbd_device * usbd_dev;

inline usbd_device * usb_setup(
    const struct usb_device_descriptor *dev,
    const struct usb_config_descriptor *conf,
    const char * const *strings, int num_strings,
    uint8_t *control_buffer, uint16_t control_buffer_size
) {
    /* Enable clocks for GPIO port A and USB peripheral. */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USB);
	/* Setup GPIO pins for USB D+/D-. */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13 | GPIO14 | GPIO15);
    gpio_set_af(GPIOB, GPIO_AF12, GPIO13 | GPIO14 | GPIO15);

    usbd_dev = usbd_init(&usb_driver, 
        dev, conf, strings, num_strings, control_buffer, control_buffer_size);
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