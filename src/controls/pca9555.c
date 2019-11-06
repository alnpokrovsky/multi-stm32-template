#include "pca9555.h"
#include "word.h"
#include <stdlib.h>

#define NXP_INPUT0     0
#define NXP_INPUT1     1
#define NXP_OUTPUT0    2
#define NXP_OUTPUT1    3

#define NXP_CONFIG0    6
#define NXP_CONFIG1    7


static void pca9555_ioset(pca9555_Conf * dev, uint16_t ioset)
{
    dev->ioSet = ioset;
    WORD2BYTES w2b;
    w2b.word = ioset;

    iic_send(dev->iicPort, dev->iicAddr, NXP_CONFIG0, w2b.byte_low);
    iic_send(dev->iicPort, dev->iicAddr, NXP_CONFIG1, w2b.byte_high);
}

void pca9555_init(pca9555_Conf * dev)
{
    pca9555_write(dev, dev->ioDefault);    
    pca9555_ioset(dev, dev->ioSet);
}


void pca9555_write(pca9555_Conf * dev, uint16_t value)
{
    WORD2BYTES w2b;
    w2b.word = value;

    iic_send(dev->iicPort, dev->iicAddr, NXP_OUTPUT0, w2b.byte_low);
    iic_send(dev->iicPort, dev->iicAddr, NXP_OUTPUT1, w2b.byte_high);
}

uint16_t pca9555_read(pca9555_Conf * dev)
{
    uint16_t inputData = 0;
    inputData = iic_recv(dev->iicPort, dev->iicAddr, NXP_INPUT0);
    inputData |= iic_recv(dev->iicPort, dev->iicAddr, NXP_INPUT1) << 8;
    return inputData;
}
