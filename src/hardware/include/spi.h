#ifndef __HARDWARE_SPI_H__
#define __HARDWARE_SPI_H__

#include <stdint.h>

typedef enum {
    SPI_1,
    SPI_2,
} SPI_PORT;

/* data frame format */
typedef enum {
    DFF_8BIT,
    DFF_16BIT,
} SPI_DFF;


void spi_init(SPI_PORT port, SPI_DFF dff);

void spi_dma_init(SPI_PORT port, uint16_t * data);

void spi_dma_start_once(SPI_PORT port, uint16_t len);

uint8_t spi_xtransfer(SPI_PORT port, uint8_t b);

#endif
