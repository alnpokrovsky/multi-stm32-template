#ifndef __HARDWARE_PWR_H__
#define __HARDWARE_PWR_H__

#include <stdint.h>
#include <stdbool.h>


void pwr_init(uint32_t scale);

void pwr_setOverdrive(bool enable);

#endif
