#ifndef __CONTROLS_PCA9555_H__
#define __CONTROLS_PCA9555_H__

#include <stdint.h>
#include "iic.h"

struct PCA9555;


struct PCA9555 * pca9555_init(IIC_PORT iic, uint8_t address);

void pca9555_config(struct PCA9555 * dev, uint16_t config);

void pca9555_write(struct PCA9555 * dev, uint16_t value);

uint16_t pca9555_read(struct PCA9555 * dev);


#endif
