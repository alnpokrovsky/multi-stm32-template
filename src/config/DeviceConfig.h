#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__

#if defined(STM32F1)
#   include "device/STM32F103C8t_config.h"
#elif defined(STM32F4)
#   include "device/STM32F4DISCO_config.h"
#elif defined(USE_MDR1986VE9x)
#   include "device/MDR32F9Qx_config.h"
#else
#   error "device not defined"
#endif


#endif