#if 0

#include "uf2.h"
#include <string.h>
#include "memflash.h"
#include "DeviceConfig.h"
#include "bootloader.h"


// All entries are little endian.
#define UF2_MAGIC_START0            0x0A324655UL // "UF2\n"
#define UF2_MAGIC_START1            0x9E5D5157UL // Randomly selected
#define UF2_MAGIC_END               0x0AB16F30UL // Ditto

// If set, the block is "comment" and should not be flashed to the device
#define UF2_FLAG_NOFLASH            0x00000001
#define UF2_FLAG_FAMILYID_PRESENT   0x00002000
#define UF2_FAMILY                  0x5ee21072



/**
 * https://www.diycode.cc/projects/Microsoft/uf2
 */
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

#define NO_CACHE 0xffffffff

static uint32_t flashAddr = NO_CACHE;
static uint8_t flashBuf[MEMFLASH_SECTOR_SIZE] __attribute__((aligned(4)));
static bool firstFlush = true;
// static bool hadWrite = false;
static uint32_t ms;
static uint32_t resetTime;
static uint32_t lastFlush;

static void ghostfat_flash_flush(void) {
    lastFlush = ms;
    if (flashAddr == NO_CACHE)
        return;

    if (firstFlush) {
        firstFlush = false;

        // disable bootloader or something
    }
    // debug_print("ghostfat_flash_flush "); debug_print_unsigned((size_t) flashAddr); debug_println(""); debug_flush();
    // DBG("Flush at %x", flashAddr);
    if (memcmp(flashBuf, (void *)flashAddr, MEMFLASH_SECTOR_SIZE) != 0) {
        // debug_print("ghostfat_flash_flush write "); debug_print_unsigned((size_t) flashAddr); debug_println(""); debug_flush();
        // DBG("Write flush at %x", flashAddr);

        // memflash_unlock();
        // bool ok = memflash_program_array((void *)flashAddr, (void*)flashBuf, MEMFLASH_SECTOR_SIZE / 2);
        // memflash_lock();
    }

    flashAddr = NO_CACHE;
}

// static void ghostfat_flash_write(uint32_t dst, const uint8_t *src, int len) {
//     uint32_t newAddr = dst & ~(MEMFLASH_SECTOR_SIZE - 1);

//     hadWrite = true;

//     if (newAddr != flashAddr) {
//         ghostfat_flash_flush();
//         flashAddr = newAddr;
//         memcpy(flashBuf, (void *)newAddr, MEMFLASH_SECTOR_SIZE);
//     }
//     memcpy(flashBuf + (dst & (MEMFLASH_SECTOR_SIZE - 1)), src, len);
// }


static void uf2_timer_start(int delay) {
    resetTime = ms + delay;
}

// called roughly every 1ms
void ghostfat_1ms() {
    ms++;

    if (resetTime && ms >= resetTime) {
        // debug_println("ghostfat_1ms target_manifest_app");  debug_flush();  ////
        ghostfat_flash_flush();
        bootloader_manifest_app();
        while (1);
    }

    if (lastFlush && ms - lastFlush > 100) {
        ghostfat_flash_flush();
    }
}


void uf2_read_flash_sector(uint8_t *data, uint32_t sectionIdx)
{
    uint32_t addr = sectionIdx * UF2_BLOCK_SIZE;
    if (addr < memflash_awailable_size()) {
        addr += APP_BASE_ADDRESS; //todo: change with APP_START address

        UF2_Block *bl = (void *)data;
        bl->magicStart0 = UF2_MAGIC_START0;
        bl->magicStart1 = UF2_MAGIC_START1;
        bl->magicEnd = UF2_MAGIC_END;
        bl->blockNo = sectionIdx;
        bl->numBlocks = memflash_awailable_size() / UF2_BLOCK_SIZE;
        bl->targetAddr = addr;
        bl->payloadSize = UF2_BLOCK_SIZE;
        memcpy(bl->data, (void *)addr, bl->payloadSize);
    }
}


void uf2_write_flash_sector(const uint8_t *data, bool quiet, WriteState *state)
{
    const UF2_Block *bl = (const void *)data;

    // if ((bl->flags & UF2_FLAG_NOFLASH) || bl->payloadSize > 256 || (bl->targetAddr & 0xff) ||
    //     bl->targetAddr < APP_BASE_ADDRESS || bl->targetAddr + bl->payloadSize > memflash_end()) {
    //     // this happens when we're trying to re-flash CURRENT.UF2 file previously
    //     // copied from a device; we still want to count these blocks to reset properly
    // } else {
    //     ghostfat_flash_write(bl->targetAddr, bl->data, bl->payloadSize);
    // }

    bool isSet = false;

    if (state && bl->numBlocks) {
        if (state->numBlocks != bl->numBlocks) {
            if ((bl->numBlocks >= UF2_BLOCKS_COUNT) || (state->numBlocks))
                state->numBlocks = 0xffffffff;
            else
                state->numBlocks = bl->numBlocks;
        }
        if (bl->blockNo < UF2_BLOCKS_COUNT) {
            uint8_t mask = 1 << (bl->blockNo % 8);
            uint32_t pos = bl->blockNo / 8;
            if (!(state->writtenMask[pos] & mask)) {
                // logval("incr", state->numWritten);
                state->writtenMask[pos] |= mask;
                state->numWritten++;
            }
            if (state->numWritten >= state->numBlocks) {
                // wait a little bit before resetting, to avoid Windows transmit error
                // https://github.com/Microsoft/uf2-samd21/issues/11
                if (!quiet) {
                    uf2_timer_start(30);
                    isSet = true;
                }
            }
        }
        //DBG("wr %d=%d (of %d)", state->numWritten, bl->blockNo, bl->numBlocks);
    }

    if (!isSet && !quiet) {
        uf2_timer_start(500);
    }
}

inline bool uf2_is_block(const void *data) {
    const UF2_Block *bl = (const UF2_Block *)data;
    return bl->magicStart0 == UF2_MAGIC_START0 && bl->magicStart1 == UF2_MAGIC_START1 &&
           bl->magicEnd == UF2_MAGIC_END;
}

inline bool uf2_is_valid(const void *data) {
    const UF2_Block *bl = (const UF2_Block *)data;
    return ((bl->flags & UF2_FLAG_FAMILYID_PRESENT) == 0) 
        || (bl->familyID == UF2_FAMILY);
}

#endif