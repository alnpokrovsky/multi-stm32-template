#include "rcc.h"
#include "tim.h"
#include "digitalpin.h"
#include "controls/modbus.h"
#include "uart.h"
#include "usb/hid.h"

//#define LED PC_2
#define LED PC_13

int main(void) {
    rcc_init();

    digitalpin_mode(LED, DIGITALPIN_OUTPUT);

    //modbus_init(1, 115200);
    usb_hid_init();

    while (1) {
        //modbus_poll();
    }
}
