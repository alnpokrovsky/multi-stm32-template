/**
 * Обертка над memflash
 * Выдает нулевым блоком дескриптор FAT16
 */

#ifndef __GHOST_FAT_H__
#define __GHOST_FAT_H__

#include <stdint.h>
#include "GhostFatConfig.h"

void ghostfat_init(void);

int ghostfat_write_block(uint32_t lba, const uint8_t *copy_from);

int ghostfat_read_block(uint32_t block_no, uint8_t *data);


#endif
