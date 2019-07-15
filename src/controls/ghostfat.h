/**
 * Обертка над memflash
 * Выдает нулевым блоком дескриптор FAT16
 */

#ifndef __GHOST_FAT_H__
#define __GHOST_FAT_H__

#include <stdint.h>
#include "memflash.h"

#define GHOSTFAT_VOLUME_LABEL       "CONFIG"
#define GHOSTFAT_SECTOR_SIZE        MEMFLASH_SECTOR_SIZE
#define GHOSTFAT_TOTAL_SECTORS      MEMFLASH_SECTORS + 1 /* NOT LESS THAN 8!!! */
#define GHOSTFAT_TOTAL_SIZE         ( GHOSTFAT_SECTOR_SIZE * GHOSTFAT_TOTAL_SECTORS )


int ghostfat_write_block(uint32_t lba, const uint8_t *copy_from);
int ghostfat_read_block(uint32_t block_no, uint8_t *data);


#endif
