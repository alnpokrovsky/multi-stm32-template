#ifndef __HARDWARE_I2C_H__
#define __HARDWARE_I2C_H__

#include <stdint.h>

typedef enum {
    IIC_1,
    IIC_2,
    IIC_3,
} IIC_PORT;

void iic_init(IIC_PORT port);

void iic_send(IIC_PORT port, uint8_t address, uint8_t reg, uint8_t value);

uint8_t iic_recv(IIC_PORT port, uint8_t address, uint8_t reg);


#endif
