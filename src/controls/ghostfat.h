#ifndef __GHOST_FAT_H__
#define __GHOST_FAT_H__

#include <stdint.h>
#include "DeviceConfig.h"

#define UF2_VERSION        "1.00"
#define UF2_NUM_BLOCKS     8000


#define PRODUCT_NAME            "STM32BLUEPILL"
#define BOARD_ID                "STM32BLUEPILL"
#define INDEX_URL               "https://visualbluepill.github.io"
#define VOLUME_LABEL            "BLUEPILL"
// where the UF2 files are allowed to write data - we allow MBR, since it seems part of the softdevice .hex file
#define USER_FLASH_START        (APP_BASE_ADDRESS)
#define USER_FLASH_END          (memflash_end())


int write_block(uint32_t lba, const uint8_t *copy_from);
int read_block(uint32_t block_no, uint8_t *data);
void ghostfat_1ms(void);

#endif
