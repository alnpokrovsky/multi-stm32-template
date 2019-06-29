#include "rcc.h"
#include "tim.h"
#include "digitalpin.h"
#include "controls/modbus.h"
#include "uart.h"

//#define LED PC_2
#define LED PC_13


int main(void) {
    rcc_init();

    digitalpin_mode(LED, DIGITALPIN_OUTPUT);

    modbus_init();

    digitalpin_set(PC_13, 1);
    //digitalpin_toggle(LED);

    while (1) {
        modbus_poll();
    }
}
