#ifndef __CONTROLS_POUCONFIG_H__
#define __CONTROLS_POUCONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include "controls/pca9555.h"
#include "controls/modbus.h"

bool pouconfig_init(void);

void pouconfig_save_default(void);

const char* pouconfig_get_usb(void);

const modbus_Conf* pouconfig_get_modbus(void);

const pca9555_Conf* pouconfig_get_io(uint8_t expanderNum);

uint8_t pouconfig_get_ioCnt(void);

#endif