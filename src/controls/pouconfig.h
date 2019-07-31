#ifndef __CONTROLS_POUCONFIG_H__
#define __CONTROLS_POUCONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include "controls/pca9555.h"
#include "controls/modbus.h"

bool pouconfig_init(void);

pca9555_Conf* pouconfig_get_io(uint8_t expanderNum);

modbus_Conf* pouconfig_get_modbus(void);

extern uint8_t pouconfig_ioCnt;

#endif