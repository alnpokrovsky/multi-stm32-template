#ifndef __HARDWARE_MEMFLASH_H__
#define __HARDWARE_MEMFLASH_H__

#include <stdint.h>
#include <stddef.h>
#include "DeviceConfig.h"

#define MEMFLASH_SECTOR_SIZE     FLASH_PAGE_SIZE
#define MEMFLASH_SECTORS         8  /* NOT LESS THAN 8!!! */

void memflash_init(void);

size_t memflash_awailable_size(void);

void memflash_read_block(uint8_t block_no, uint8_t *data);
void memflash_write_block(uint8_t block_no, const uint8_t *data);


#endif