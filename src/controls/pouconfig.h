#ifndef __CONTROLS_POUCONFIG_H__
#define __CONTROLS_POUCONFIG_H__

#include <stdint.h>
#include "controls/pca9555.h"

void pouconfig_init(void);

PCA9555_config* pouconfig_get_io(uint8_t expanderNum);

extern uint8_t pouconfig_ioCnt;

#endif