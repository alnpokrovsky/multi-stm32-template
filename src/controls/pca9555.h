#ifndef __CONTROLS_PCA9555_H__
#define __CONTROLS_PCA9555_H__

#include <stdint.h>
#include "iic.h"

struct PCA9555;

typedef struct {
    IIC_PORT iicPort;
    uint8_t iicAddr;
    uint16_t ioSet;
} pca9555_Conf;

struct PCA9555 * pca9555_init(const pca9555_Conf * conf);

//void pca9555_ioset(struct PCA9555 * dev, uint16_t ioset);

void pca9555_write(struct PCA9555 * dev, uint16_t value);

uint16_t pca9555_read(struct PCA9555 * dev);


#endif
