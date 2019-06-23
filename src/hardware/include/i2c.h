#ifndef __HARDWARE_I2C_H__
#define __HARDWARE_I2C_H__

#include "byte.h"

void i2c_init(void);

void i2c_send(byte address, byte reg, byte value);

word i2c_recv(byte address, byte reg);


#endif
