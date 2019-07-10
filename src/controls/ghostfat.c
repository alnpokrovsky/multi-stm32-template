#include "ghostfat.h"

#include <string.h>

#include "memflash.h"
#include "bootloader.h"
#include "uf2.h"

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

typedef struct {
    char name[FILE_NAME_SIZE];
    char ext[FILE_EXT_SIZE];
    uint8_t attrs;
    uint8_t reserved;
    uint8_t createTimeFine;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t highStartCluster;
    uint16_t updateTime;
    uint16_t updateDate;
    uint16_t startCluster;
    uint32_t size;
} __attribute__((packed)) DirEntry;

/**
 * fill empty with spaces while copying fat16 filename
 */
static void padded_memcpy(char *dst, const char *src) {
    for (int i = 0; i < FILE_FULLNAME_SIZE; ++i) {
        if (*src)
            *dst = *src++;
        else
            *dst = ' ';
        dst++;
    }
}

/////////////////////////// fat16 load sector //////////
#define RESERVED_SECTORS    1
#define ROOT_DIR_SECTORS    1
#define SECTORS_PER_FAT     1
#define SECTORS_PER_CLUSTER 2
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
    .SectorSize = SECTOR_SIZE,
    .SectorsPerCluster = SECTORS_PER_CLUSTER,
    .ReservedSectors = RESERVED_SECTORS,
    .FATCopies = 2,
    .RootDirectoryEntries = (ROOT_DIR_SECTORS * SECTOR_SIZE / 32),
    .TotalSectors16 = FLASH_TOTAL_SECTORS,
    .MediaDescriptor = MEDIA_DESCRIPTOR,
    .SectorsPerFAT = SECTORS_PER_FAT,
    .SectorsPerTrack = 1,
    .Heads = 1,
    .ExtendedBootSig = 0x29,
    .VolumeSerialNumber = 0x00420042,
    .VolumeLabel = VOLUME_LABEL,
    .FilesystemIdentifier = "FAT16   ",
};

//////////////////////// FILES //////////////////////

typedef struct {
    const char name[FILE_FULLNAME_SIZE];
    const char *content;
} TextFile;

const char infoFile[] = //
    "Model: "         PRODUCT_NAME  "\r\n"
    "Board-ID: "      BOARD_ID      "\r\n";

const char indexFile[] = //
    "<!doctype html>\n"
    "<html>"
    "<body>"
    "<script>\n"
    "location.replace(\"" INDEX_URL "\");\n"
    "</script>"
    "</body>"
    "</html>\n";

static const TextFile files[] = {
    {.name = "INFO    TXT", .content = infoFile},
    {.name = "INDEX   HTM", .content = indexFile},
    {.name = "CURRENT UF2"},
};
#define FILES_COUNT ( sizeof(files) / sizeof(files[0]) )

#define UF2_FIRST_SECTOR    ( FILES_COUNT + 1 )
#define UF2_LAST_SECTOR     ( UF2_FIRST_SECTOR + UF2_BLOCKS_COUNT - 1 )

/////////////////////////////////////////////////////


static void fat16_boot_sector(uint8_t *data)
{
    memcpy(data, &BootBlock, sizeof(BootBlock));
    /* bootSignature 0x55AA 
    * - конец загрузочного сектора */
    data[SECTOR_SIZE-2] = 0x55;
    data[SECTOR_SIZE-1] = 0xaa;
}

#define FAT_COLUMNS_COUNT   ( SECTORS_PER_FAT * SECTOR_SIZE / 2 )
#define FAT_EOF             0xffff
#define FAT_LABEL_OFFSET    1

#define FAT_ATTR_READONLY   0x01
#define FAT_ATTR_LABEL      0x28

static void fat16_fat_sector(uint8_t *data, uint32_t block_no)
{
    (void)block_no; //todo: use if fat sector more than one block

    /* дескриптор таблицы */
    data[0] = MEDIA_DESCRIPTOR;
    data[1] = 0xFF;

    uint16_t * wdata = (uint16_t *)data;
    wdata[FAT_LABEL_OFFSET] = FAT_EOF; // метка тома

    //todo: rewrite
    /* Каждая строка таблицы FAT занимает 2 байта памяти, 
     * поэтому количество строк для нашего случая это 512/2 */
    for (uint16_t i = 2; i < FAT_COLUMNS_COUNT; ++i)
    {
        /* блоки маленьких файлов */
        if (i <= FAT_LABEL_OFFSET + FILES_COUNT) {
            wdata[i] = FAT_EOF;
        }
        /* блок памяти UF2 */
        else if (FAT_LABEL_OFFSET + UF2_FIRST_SECTOR <= i && i <= FAT_LABEL_OFFSET + UF2_LAST_SECTOR)
            wdata[i] = (i == UF2_LAST_SECTOR) ? FAT_EOF : i + 1;
    }
}

static void fat16_root_sector(uint8_t *data)
{
    DirEntry *d = (void *)data;
    padded_memcpy(d->name, (const char *)BootBlock.VolumeLabel);
    d->attrs = FAT_ATTR_LABEL; // метка тома
    for (uint16_t i = 1; i <= FILES_COUNT; ++i) {
        d++;
        // имя файла
        padded_memcpy(d->name, files[i].name);
        // размер файла
        if (files[i].content != NULL) {
            d->size = strlen(files[i].content);
        } else {
            d->size = memflash_awailable_size(); //todo: тут должен быть размер прошивки
        }
        /**
         * номер первого кластера
         * указывает на слово в таблице FAT1
         * (файлы идут по порядку и занимают всего 
         * по одному кластеру)
         */
        d->startCluster = i + FAT_LABEL_OFFSET;
    }
}

int ghostfat_read_block(uint32_t block_no, uint8_t *data)
{
    memset(data, 0, SECTOR_SIZE);
    uint32_t sectionIdx = block_no;

    if (block_no == 0) // загрузочный сектор
    {
        fat16_boot_sector(data);
    }
    else if (block_no < START_ROOTDIR) // Fat0/Fat1
    {
        fat16_fat_sector(data, block_no - START_FAT0);
    }
    else if (block_no == START_ROOTDIR) // корневой каталог
    {
        fat16_root_sector(data);
    }
    else { // данные
        sectionIdx -= START_CLUSTERS;
        if (sectionIdx < FILES_COUNT - 1) {
            memcpy(data, files[sectionIdx].content, strlen(files[sectionIdx].content));
        } else { // прошивка
            sectionIdx -= FILES_COUNT - 1;
            uf2_read_flash_sector(data, sectionIdx);
        }
    }

    return 0;
}


int ghostfat_write_block(uint32_t lba, const uint8_t *copy_from)
{
    (void)lba;

    if (uf2_is_block(copy_from)) // если пишем прошивку
    {
        if (uf2_is_valid(copy_from)) {
            static WriteState wrState;
            uf2_write_flash_sector(copy_from, false, &wrState);
        } else {
            return -1;
        }
    }
    else { // если обычный файл

    }

    return 0;
}
