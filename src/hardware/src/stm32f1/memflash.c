#if defined(STM32F1)

#include "memflash.h"
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <string.h>
#include "DeviceConfig.h"


static bool memflash_write_page(uint32_t page_address, const uint8_t data[])
{
    // проверяем: если данные не изменились, перезаписывать их не будем
    if (memcmp(data, (uint8_t *)page_address, MEMFLASH_SECTOR_SIZE) == 0)
        return true;

    /*Erasing page*/
	flash_erase_page(page_address);
	if(flash_get_status_flags() != FLASH_SR_EOP)
		return false;

    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i += 4) {
        flash_program_word(page_address + i, *((uint32_t*)(data + i)));
		if(flash_get_status_flags() != FLASH_SR_EOP)
		 	return false;

		/*verify if correct data is programmed*/
		if(*((uint32_t*)(page_address+i)) != *((uint32_t*)(data + i)))
			return false;
    }

    return true;
}

static inline uint32_t memflash_end(void) {
    /* Only allow access to the chip's self-reported flash size */
    return (FLASH_BASE + DESIG_FLASH_SIZE*FLASH_PAGE_SIZE);
}

/**
 * count blocks from end of flash where must be no
 * program blocks to override
 */
static inline uint32_t memflash_page_addr(uint8_t block) {
    return  memflash_end() - (block + 1) * FLASH_PAGE_SIZE;
}


/**
 * read from inner flash memory
 * 
 * \param block_no номер блока флеш-памяти микроконтроллера 
 * \param[out] data массив размера MEMFLASH_SECTOR_SIZE
 */
void memflash_read_block(uint8_t block_no, uint8_t *data)
{
    uint32_t page_addr = memflash_page_addr(block_no);
    memcpy(data, (uint8_t *)page_addr, FLASH_PAGE_SIZE);
}

/**
 * \param block_no номер блока флеш-памяти микроконтроллера 
 * \param[in] data массив размера MEMFLASH_SECTOR_SIZE
 */
void memflash_write_block(uint8_t block_no, const uint8_t *data)
{
    uint32_t page_addr = memflash_page_addr(block_no);
    flash_unlock();
    memflash_write_page(page_addr, data);
    flash_lock();
}

#endif