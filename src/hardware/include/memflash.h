#ifndef __HARDWARE_MEMFLASH_H__
#define __HARDWARE_MEMFLASH_H__

#include <stdint.h>
#include "DeviceConfig.h"

#define MEMFLASH_SECTOR_SIZE     FLASH_PAGE_SIZE
#define MEMFLASH_SECTORS         7

/**
 * init flush timer and read flash memory to RAM cash
 */
void memflash_init(void);

void memflash_read_block(uint8_t block_no, uint8_t *data);
void memflash_write_block(uint8_t block_no, const uint8_t *data);


#endif