#ifndef __CONTROLS_POUCONFIG_H__
#define __CONTROLS_POUCONFIG_H__

#include <stdint.h>
#include "controls/pca9555.h"
#include "controls/modbus.h"

#define POUCONFIG_MAX_IO  4

typedef struct {
    modbus_Conf modbus;
    uint8_t ioCnt;
    pca9555_Conf io[POUCONFIG_MAX_IO];
} POUCONFIG;


POUCONFIG * pouconfig_init(void);

void pouconfig_save(const POUCONFIG * conf);

const char* pouconfig_get_usb(void);


#endif