#if defined(STM32F0) || defined(STM32F1) || defined(USE_MDR1986VE9x)
#include "../port/ARM_CM3/portmacro.h"
#elif defined(STM32F3) || defined(STM32F4)
#include "../port/ARM_CM4F/portmacro.h"
#elif defined(STM32F7)
#include "../port/ARM_CM7/r0p1/portmacro.h"
#else
#error "You did not port your architect"
#endif