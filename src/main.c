#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "delay.h"

//#define LED PC_2
#define LED PC_13

int main(void) {
    rcc_init();

    digitalpin_mode(LED, DIGITALPIN_OUTPUT);

    usb_init();

    while (1) {
        digitalpin_toggle(LED);
        delay_some();
        delay_some();
        delay_some();
        delay_some();
        delay_some();
    }
}
