#if defined(STM32F1)

#include "memflash.h"
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <string.h>
#include "tim.h"

#define MEMFLASH_FLUSH_TIM       TIM_4
#define MEMFLASH_FLUSH_HANDLER   tim4_handler


typedef uint8_t sector[MEMFLASH_SECTOR_SIZE];


static bool memflash_write_page(uint32_t page_address, const sector data)
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


static sector cash[MEMFLASH_SECTORS];
static bool initedFlag = false;

void memflash_init(void) {
    if (!initedFlag) { 
        initedFlag = true;

        for (uint8_t block = 0; block < MEMFLASH_SECTORS; ++block) {
            uint32_t page_addr = memflash_page_addr(block);
            memcpy(&cash[block], (uint8_t *)page_addr, FLASH_PAGE_SIZE);
        }

        tim_init(MEMFLASH_FLUSH_TIM, 50000, MICROSEC);
    }
}

/**
 * read from cashed memory
 * 
 * \param block_no номер блока флеш-памяти микроконтроллера 
 * \param[out] data массив размера MEMFLASH_SECTOR_SIZE
 */
void memflash_read_block(uint8_t block_no, uint8_t *data)
{
    memcpy(data, &cash[block_no], FLASH_PAGE_SIZE);
}

/**
 * write to cash and restart flush timer
 * when timer is ready, write cash to flash
 * 
 * \param block_no номер блока флеш-памяти микроконтроллера 
 * \param[in] data массив размера MEMFLASH_SECTOR_SIZE
 */
void memflash_write_block(uint8_t block_no, const uint8_t *data)
{
    memcpy(&cash[block_no], data, FLASH_PAGE_SIZE);
    
    tim_stop(MEMFLASH_FLUSH_TIM);
    tim_start_once(MEMFLASH_FLUSH_TIM);
}

void MEMFLASH_FLUSH_HANDLER() {
    flash_unlock();
    for (uint8_t block = 0; block < MEMFLASH_SECTORS; ++block) {
        uint32_t page_addr = memflash_page_addr(block);
        memflash_write_page(page_addr, cash[block]);
    }
    flash_lock();
}

#endif