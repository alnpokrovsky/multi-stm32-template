#include "rcc.h"
#include "digitalpin.h"
#include "controls/rtos.h"
#include "delay.h"

// static void vLed1Task(void * arg) {
//     (void)arg;
//     digitalpin_mode(PB_6, DIGITALPIN_OUTPUT);
//     RTOS_DELAY_LOOP {
//         digitalpin_toggle(PB_6);
//         RTOS_DELAY_NEXT_MS( 500 );
//     }
// }


#include "usb_core.h"

int main(void) {
    rcc_init();

    usb_core_init();

    // RTOS_TASK_CREATE(RTOS_LOW_PRIORITY, vLed1Task);

    // RTOS_TASK_CREATE(RTOS_HIGH_PRIORITY+5, vLed2Task);

    RTOS_START();

    while (1);
}

