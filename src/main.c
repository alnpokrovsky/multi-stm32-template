#include "rcc.h"
#include "digitalpin.h"
#include "controls/rtos.h"
#include "controls/stm32f3/hrpwm.h"

#define BOOT1 PB_2

static void vSinTask(void * arg) {
    (void)arg;
    hrpwm_init();
}


int main(void) {
    rcc_init();

    RTOS_TASK(vSinTask, 0, RTOS_HIGH_PRIORITY);
    
    RTOS_START();

    while (1);
}
