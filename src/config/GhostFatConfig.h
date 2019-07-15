#ifndef __GHOSTFAT_CONFIG_H__
#define __GHOSTFAT_CONFIG_H__

#include "memflash.h"

#define GHOSTFAT_VOLUME_LABEL       "CONFIG"
#define GHOSTFAT_SECTOR_SIZE        MEMFLASH_SECTOR_SIZE
#define GHOSTFAT_TOTAL_SECTORS      MEMFLASH_SECTORS + 1 /* NOT LESS THAN 8!!! */
#define GHOSTFAT_TOTAL_SIZE         ( GHOSTFAT_SECTOR_SIZE * GHOSTFAT_TOTAL_SECTORS )

#define GHOSTFAT_FLASH_INIT         memflash_init    
#define GHOSTFAT_FLASH_WRITE_BLOCK  memflash_write_block
#define GHOSTFAT_FLASH_READ_BLOCK   memflash_read_block

#endif