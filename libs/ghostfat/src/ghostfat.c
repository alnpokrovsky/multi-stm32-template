#include "ghostfat.h"
#include "tim.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


#define FILE_NAME_SIZE      8
#define FILE_EXT_SIZE       3
#define FILE_FULLNAME_SIZE  ( FILE_NAME_SIZE + FILE_EXT_SIZE )

typedef struct {
    uint8_t JumpInstruction[3];
    uint8_t OEMInfo[8];
    uint16_t SectorSize;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t RootDirectoryEntries;
    uint16_t TotalSectors16;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t TotalSectors32;
    uint8_t PhysicalDriveNum;
    uint8_t Reserved;
    uint8_t ExtendedBootSig;
    uint32_t VolumeSerialNumber;
    uint8_t VolumeLabel[FILE_FULLNAME_SIZE];
    uint8_t FilesystemIdentifier[8];
} __attribute__((packed)) FAT_BootBlock;


/////////////////////////// fat16 load sector //////////
#define RESERVED_SECTORS    1
#define ROOT_DIR_SECTORS    1
#define SECTORS_PER_FAT     1
#define SECTORS_PER_CLUSTER 1
#define MEDIA_DESCRIPTOR    0xF8

#define START_FAT0          RESERVED_SECTORS
#define START_FAT1          ( START_FAT0 + SECTORS_PER_FAT )
#define START_ROOTDIR       ( START_FAT1 + SECTORS_PER_FAT )
#define START_CLUSTERS      ( START_ROOTDIR + ROOT_DIR_SECTORS )

/**
 * http://s-engineer.ru/opisanie-fajlovoj-sistemy-fat16/
 */
static const FAT_BootBlock BootBlock = {
    .JumpInstruction = {0xeb, 0x3c, 0x90},
    .OEMInfo = "UF2 UF2 ",
    .SectorSize = GHOSTFAT_SECTOR_SIZE,
    .SectorsPerCluster = SECTORS_PER_CLUSTER,
    .ReservedSectors = RESERVED_SECTORS,
    .FATCopies = 1,
    .RootDirectoryEntries = (ROOT_DIR_SECTORS * GHOSTFAT_SECTOR_SIZE / 32),
    .TotalSectors16 = GHOSTFAT_TOTAL_SECTORS,
    .MediaDescriptor = MEDIA_DESCRIPTOR,
    .SectorsPerFAT = SECTORS_PER_FAT,
    .SectorsPerTrack = 1,
    .Heads = 1,
    .ExtendedBootSig = 0x29,
    .VolumeSerialNumber = 0x00420042,
    .VolumeLabel = GHOSTFAT_VOLUME_LABEL,
    .FilesystemIdentifier = "FAT16   ",
};


static void fat16_boot_sector(uint8_t *data)
{
    memset(data, 0, GHOSTFAT_SECTOR_SIZE - 2);

    memcpy(data, &BootBlock, sizeof(BootBlock));
    /* bootSignature 0x55AA 
    * - конец загрузочного сектора */
    data[510] = 0x55;
    data[511] = 0xaa;
}

typedef uint8_t sector[GHOSTFAT_SECTOR_SIZE];

static sector * cache;
static bool initedCache = false;

void ghostfat_init(void) {
    if (!initedCache) { 
        initedCache = true;
        cache = malloc(sizeof(sector) * MEMFLASH_SECTORS);
        for (uint8_t block = 0; block < MEMFLASH_SECTORS; ++block) {
            GHOSTFAT_FLASH_READ_BLOCK(block, cache[block]);
        }

        tim_init(GHOSTFAT_FLUSH_TIM, 50000, MICROSEC);
    }
}

void ghostfat_deinit(void) {
    free(cache);
    initedCache = false;
}

int ghostfat_read_block(uint32_t block_no, uint8_t *data)
{
    if (block_no == 0) {
        // загрузочный сектор
        fat16_boot_sector(data);
    } else if(initedCache) {
        memcpy(data, &cache[block_no-1], GHOSTFAT_SECTOR_SIZE);
    } else {
        GHOSTFAT_FLASH_READ_BLOCK(block_no-1, data);        
    }

    return 0;
}

/**
 * write to cache and restart flush timer
 * when timer is ready, write cache to flash
 */
int ghostfat_write_block(uint32_t block_no, const uint8_t *data)
{
    if (block_no == 0) {
        // не даем перезаписывать загрузочный сектор
        return 0;
    } else if (initedCache) {
        assert(initedCache);    
        memcpy(&cache[block_no-1], data, GHOSTFAT_SECTOR_SIZE);
        
        tim_stop(GHOSTFAT_FLUSH_TIM);
        tim_start_once(GHOSTFAT_FLUSH_TIM);
    } else {
        GHOSTFAT_FLASH_WRITE_BLOCK(block_no-1, data);
    }

    return 0;
}

void GHOSTFAT_FLUSH_HANDLER() {
    for (uint8_t block = 0; block < MEMFLASH_SECTORS; ++block) {
        GHOSTFAT_FLASH_WRITE_BLOCK(block, cache[block]);
    }
}
