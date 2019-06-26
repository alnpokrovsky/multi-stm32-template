#if defined(USE_MDR1986VE9x)

#include "rcc.h"
#include "MDR32Fx.h"
#include "system_MDR32F9Qx.h"

void rcc_init(void) {
    SystemInit();//Системная функция, которая инициализирует тактовый генератор
}

#endif