#include "rcc.h"
#include "digitalpin.h"
#include "controls/rtos.h"


// static void vSinTask(void * arg) {
//     (void)arg;
//     hrpwm_init();
// }


int main(void) {
    rcc_init();

    
    // RTOS_START();

    while (1);
}
