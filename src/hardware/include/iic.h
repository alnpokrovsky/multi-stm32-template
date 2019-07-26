#ifndef __HARDWARE_I2C_H__
#define __HARDWARE_I2C_H__

#include "byte.h"

typedef enum {
    IIC_1,
    IIC_2,
} IIC_PORT;

void iic_init(IIC_PORT port);

void iic_send(IIC_PORT port, byte address, byte reg, byte value);

word iic_recv(IIC_PORT port, byte address, byte reg);


#endif
