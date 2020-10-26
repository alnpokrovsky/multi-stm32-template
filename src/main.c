#include "rcc.h"
#include <cmsis_os2.h>
#include "usb_device/app/usb_device.h"
#include "gpio.h"


int main(void) {
    rcc_init();

    MX_USB_DEVICE_Init();

    osKernelInitialize();

    // power on
    GPIO_Pins PwrPin = {GPIO_PORT_E, GPIO_PIN_2, GPIO_PUPD_NONE, GPIO_MODE_OUTPUT, 0};
    gpio_init(&PwrPin);
    gpio_set(&PwrPin, GPIO_PINS_ON);

    

    // osThreadNew(app_led1, NULL, NULL);

    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }
    while (1);
}

