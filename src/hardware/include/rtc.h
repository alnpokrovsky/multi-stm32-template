#ifndef __HARDWARE_RTC_H__
#define __HARDWARE_RTC_H__

#include <stdint.h>

void rtc_init(void);


void rtc_handler(uint32_t cnt);

#endif
