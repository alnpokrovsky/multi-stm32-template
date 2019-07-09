#ifndef __LIB_USB_PRIVATE_UF2FORMAT_H__
#define __LIB_USB_PRIVATE_UF2FORMAT_H__

#include "UsbConfig.h"
#include "DeviceConfig.h"

#include <stdint.h>
#include <stdbool.h>

#define UF2_VERSION        "1.00"
#define UF2_NUM_BLOCKS     8000

// All entries are little endian.
#define UF2_MAGIC_START0            0x0A324655UL // "UF2\n"
#define UF2_MAGIC_START1            0x9E5D5157UL // Randomly selected
#define UF2_MAGIC_END               0x0AB16F30UL // Ditto

// If set, the block is "comment" and should not be flashed to the device
#define UF2_FLAG_NOFLASH            0x00000001
#define UF2_FLAG_FAMILYID_PRESENT   0x00002000

#define UF2_IS_MY_FAMILY(bl)                                                                       \
    (((bl)->flags & UF2_FLAG_FAMILYID_PRESENT) == 0 || (bl)->familyID == UF2_FAMILY)

#define MAX_BLOCKS                  256

typedef struct {
    uint32_t numBlocks;
    uint32_t numWritten;
    uint8_t writtenMask[MAX_BLOCKS / 8 + 1];
} WriteState;

typedef struct {
    // 32 byte header
    uint32_t magicStart0;
    uint32_t magicStart1;
    uint32_t flags;
    uint32_t targetAddr;
    uint32_t payloadSize;
    uint32_t blockNo;
    uint32_t numBlocks;
    uint32_t familyID;

    // raw data;
    uint8_t data[476];

    // store magic also at the end to limit damage from partial block reads
    uint32_t magicEnd;
} UF2_Block;

typedef struct {
    uint8_t version;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t reserved0;
    uint32_t cbw_tag;
    uint32_t blocks_remaining;
    uint8_t *buffer;
} UF2_HandoverArgs;

int write_block(uint32_t lba, const uint8_t *copy_from);
int read_block(uint32_t block_no, uint8_t *data);
void ghostfat_1ms(void);

typedef void (*UF2_MSC_Handover_Handler)(UF2_HandoverArgs *handover);
typedef void (*UF2_HID_Handover_Handler)(int ep);

// this is required to be exactly 16 bytes long by the linker script
typedef struct {
    void *reserved0;
    UF2_HID_Handover_Handler handoverHID;
    UF2_MSC_Handover_Handler handoverMSC;
    const char *info_uf2;
} UF2_BInfo;

#define UF2_BINFO ((UF2_BInfo *)(APP_START_ADDRESS - sizeof(UF2_BInfo)))

static inline bool is_uf2_block(const void *data) {
    const UF2_Block *bl = (const UF2_Block *)data;
    return bl->magicStart0 == UF2_MAGIC_START0 && bl->magicStart1 == UF2_MAGIC_START1 &&
           bl->magicEnd == UF2_MAGIC_END;
}

#endif
