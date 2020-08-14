#ifndef __UF2_FORMAT_H__
#define __UF2_FORMAT_H__

#include <stdbool.h>
#include <stdint.h>

#define UF2_BLOCK_SIZE              256
#define UF2_BLOCKS_COUNT            100

typedef struct {
    uint32_t numBlocks;
    uint32_t numWritten;
    uint8_t writtenMask[UF2_BLOCKS_COUNT / 8 + 1];
} WriteState;

bool uf2_is_block(const void *data);
bool uf2_is_valid(const void *data);

void uf2_read_flash_sector(uint8_t *data, uint32_t sectionIdx);
void uf2_write_flash_sector(const uint8_t *data, bool quiet, WriteState *state);


#endif