#include "rcc.h"
#include "usb/hid-cdc.h"


int main(void) {
    rcc_init();

    hid_cdc_init();

    while (1);
}
