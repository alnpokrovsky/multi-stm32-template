#ifndef __HARDWARE_SPIBUS_H__
#define __HARDWARE_SPIBUS_H__

#include <stdint.h>

typedef enum {
    SPI_1,
    SPI_2,
    SPI_3,
    SPI_4,
    SPI_5,
} SPIBUS_PORT;

/* data frame format */
typedef enum {
    DFF_8BIT,
    DFF_16BIT,
} SPIBUS_DFF;


void spibus_init(SPIBUS_PORT port, SPIBUS_DFF dff);

void spibus_dma_init(SPIBUS_PORT port, uint16_t * data);

void spibus_dma_start_once(SPIBUS_PORT port, uint16_t len);

uint16_t spibus_xfer(SPIBUS_PORT port, uint16_t b);

#endif
