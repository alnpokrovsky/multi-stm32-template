#ifndef __CONTROLS_PCA9555_H__
#define __CONTROLS_PCA9555_H__

#include <stdint.h>
#include "iic.h"

typedef struct {
    IIC_PORT iicPort;
    uint8_t iicAddr;
    uint16_t ioSet;
    uint16_t ioDefault;
} pca9555_Conf;

void pca9555_init(pca9555_Conf * dev);

void pca9555_write(pca9555_Conf * dev, uint16_t value);

uint16_t pca9555_read(pca9555_Conf * dev);


#endif
