#include "rcc.h"
#include "digitalpin.h"
#include "controls/rtos.h"


// static void vSinTask(void * arg) {
//     (void)arg;
//     hrpwm_init();
// }

static void vLedTask(void * arg) {
    (void) arg;
    digitalpin_mode(PC_2, DIGITALPIN_OUTPUT);
    // digitalpin_set(PC_2, true);
    while (1) {
        portTickType xLastWakeTime = xTaskGetTickCount();

        digitalpin_toggle(PC_2);

        vTaskDelayUntil( &xLastWakeTime, 1000 );
    }
}

int main(void) {
    rcc_init();

    RTOS_TASK(RTOS_MEDIUM_PRIORITY, vLedTask, 0);
    RTOS_START();

    while (1);
}
