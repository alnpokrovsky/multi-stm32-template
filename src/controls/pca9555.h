#ifndef __CONTROLS_PCA9555_H__
#define __CONTROLS_PCA9555_H__

#include <stdint.h>
#include "iic.h"

struct PCA9555;

typedef struct {
    IIC_PORT iicPort;
    uint8_t iicAddr;
    uint16_t io;
} PCA9555_config;

struct PCA9555 * pca9555_init(PCA9555_config * conf);

//void pca9555_io(struct PCA9555 * dev, uint16_t io);

void pca9555_write(struct PCA9555 * dev, uint16_t value);

uint16_t pca9555_read(struct PCA9555 * dev);


#endif
