#include "usb_core.h"
#include "App/usb_device.h"

void usb_core_init(void) {
    MX_USB_DEVICE_Init();
}
