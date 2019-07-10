#ifndef __GHOST_FAT_H__
#define __GHOST_FAT_H__

#include <stdint.h>
#include "DeviceConfig.h"

#define SECTOR_SIZE         512 // still fixed in msc.c (look todo's)
#define FLASH_SIZE          ( 64 * 1024 ) // 64kb
#define FLASH_TOTAL_SECTORS ( FLASH_SIZE / SECTOR_SIZE )


#define PRODUCT_NAME            "STM32BLUEPILL"
#define BOARD_ID                "STM32BLUEPILL"
#define INDEX_URL               "https://visualbluepill.github.io"
#define VOLUME_LABEL            "BLUEPILL"


int ghostfat_write_block(uint32_t lba, const uint8_t *copy_from);
int ghostfat_read_block(uint32_t block_no, uint8_t *data);


#endif
