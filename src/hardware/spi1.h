#ifndef __HARDWARE_SPI_1_H__
#define __HARDWARE_SPI_1_H__

#include <stdint.h>

/**
 * SS = PA4
 * SCK = PA5
 * MISO = PA6
 * MOSI = PA7
 */
void spi1_init(void);

void spi1_send(uint8_t b);

uint8_t spi1_read(void);

#endif
