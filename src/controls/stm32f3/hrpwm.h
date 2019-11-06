#ifndef __CONTROLS_HRPWM_H__
#define __CONTROLS_HRPWM_H__

#ifndef STM32F3
#error "not supported by other than stm32f3 series"
#endif

void hrpwm_init(void);

#endif