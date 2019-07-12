#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "memflash.h"
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <string.h>
#include "critical.h"
#include "tim.h"

#define FLUSH_MEM_TIM TIM_4

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

static inline uint32_t memflash_page_addr(uint8_t block) {
    return  memflash_end() - (block + 1) * FLASH_PAGE_SIZE;
}


size_t memflash_awailable_size(void) {
    uint8_t* flash_end = (uint8_t*)memflash_end();
    uint8_t* flash_start = (uint8_t*)(APP_BASE_ADDRESS);

    return (flash_end >= flash_start) ? (size_t)(flash_end - flash_start) : 0;
}


static sector wbuffer[MEMFLASH_SECTORS];
static volatile bool writeFlag = false;

void memflash_init(void) {
    for (uint8_t block = 1; block < MEMFLASH_SECTORS; ++block) {
        uint32_t page_addr = memflash_page_addr(block);
        memcpy(&wbuffer[block], (uint8_t *)page_addr, FLASH_PAGE_SIZE);
    }

    tim_init(FLUSH_MEM_TIM, 50000, MICROSEC);
}

/**
 * прочитать страницу флеша
 * нумерация с нуля
 * первый пишется в конец доступной памяти
 */
void memflash_read_block(uint8_t block_no, uint8_t *data)
{
    memcpy(data, &wbuffer[block_no], FLASH_PAGE_SIZE);
}


void memflash_write_block(uint8_t block_no, const uint8_t *data)
{
    memcpy(&wbuffer[block_no], data, FLASH_PAGE_SIZE);
    writeFlag = true;
    tim_stop(FLUSH_MEM_TIM);
    tim_start_once(FLUSH_MEM_TIM);
}

void tim4_handler() {
    if (writeFlag) {
        flash_unlock();
        for (uint8_t block = 1; block < MEMFLASH_SECTORS; ++block) {
        	uint32_t page_addr = memflash_page_addr(block);
            memflash_write_page(page_addr, wbuffer[block]);
        }
        flash_lock();
        writeFlag = false;
    }
}

#endif