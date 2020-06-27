#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__

#if defined(STM32F1) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7)
#   include "device/STM32.h"
#elif defined(USE_MDR1986VE9x)
#   include "device/MDR32F9Qx_config.h"
#else
#   error "device not defined"
#endif


#endif