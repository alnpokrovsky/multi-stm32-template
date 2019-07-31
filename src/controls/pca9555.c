#include "pca9555.h"
#include "word.h"
#include <stdlib.h>

#define NXP_INPUT0     0
#define NXP_INPUT1     1
#define NXP_OUTPUT0    2
#define NXP_OUTPUT1    3

#define NXP_CONFIG0    6
#define NXP_CONFIG1    7

struct PCA9555 {
    IIC_PORT iic;
    uint8_t address;
    WORD2BYTES io;
    WORD2BYTES value;
};

static void pca9555_io(struct PCA9555 * dev, uint16_t io)
{
    dev->io.word = io;

    iic_send(dev->iic, dev->address, NXP_CONFIG0, dev->io.byte_low);
    iic_send(dev->iic, dev->address, NXP_CONFIG1, dev->io.byte_high);
}

struct PCA9555 * pca9555_init(PCA9555_config * conf)
{
    struct PCA9555 * dev = malloc(sizeof(*dev));
    dev->iic = conf->iicPort;
    dev->address = conf->iicAddr;
    pca9555_write(dev, 0x0000);    
    pca9555_io(dev, conf->io);
    return dev;
}


void pca9555_write(struct PCA9555 * dev, uint16_t value)
{
    dev->value.word = value;
    iic_send(dev->iic, dev->address, NXP_OUTPUT0, dev->value.byte_low);
    iic_send(dev->iic, dev->address, NXP_OUTPUT1, dev->value.byte_high);
}

uint16_t pca9555_read(struct PCA9555 * dev)
{
    uint16_t inputData = 0;
    inputData = iic_recv(dev->iic, dev->address, NXP_INPUT0);
    inputData |= iic_recv(dev->iic, dev->address, NXP_INPUT1) << 8;
    return inputData;
}
