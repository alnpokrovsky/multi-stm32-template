#if defined(STM32F7)

#include <stdint.h>
#include <stm32f7xx.h>


static void __attribute__ ((constructor)) premainInitCach(void) {
    SCB_EnableICache();
    SCB_EnableDCache();
}

#endif
