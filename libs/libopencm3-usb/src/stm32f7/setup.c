#if defined(STM32F7)

#include "../private/setup.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define usb_driver otgfs_usb_driver
#define NVIC_USB_RX NVIC_OTG_FS_IRQ
#define USB_RX_HANDLER otg_fs_isr


static usbd_device * usbd_dev;

inline usbd_device * usb_setup(
    const struct usb_device_descriptor *dev,
    const struct usb_config_descriptor *conf,
    const char * const *strings, int num_strings,
    uint8_t *control_buffer, uint16_t control_buffer_size
) {
    /* Enable clocks for GPIO port A and USB peripheral. */
	rcc_periph_clock_enable(RCC_GPIOA);
    /* Setup GPIO pins for USB D+/D-. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO12);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	RCC_AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
    while ( !(RCC_AHB2ENR & RCC_AHB2ENR_OTGFSEN) ) ;

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