#ifndef __HARDWARE_MEMFLASH_H__
#define __HARDWARE_MEMFLASH_H__

#ifndef STM32F1
#error "supported only STM32F1 for now"
#endif

#include <stdint.h>
#include "DeviceConfig.h"

#define MEMFLASH_SECTOR_SIZE     FLASH_PAGE_SIZE
#define MEMFLASH_SECTORS         7

void memflash_read_block(uint8_t block_no, uint8_t *data);
void memflash_write_block(uint8_t block_no, const uint8_t *data);


#endif