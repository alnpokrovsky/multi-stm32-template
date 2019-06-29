#include "rcc.h"
#include "tim.h"
#include "digitalpin.h"

void tim1_handler() {
    digitalpin_toggle(PC_2);
    //tim_stop(TIM_1);
}

int main(void) {
    rcc_init();

    digitalpin_mode(PC_2, DIGITALPIN_OUTPUT);

    tim_init(TIM_1, MILLISEC);
    tim_start(TIM_1, 1000);

    while (1);
}
