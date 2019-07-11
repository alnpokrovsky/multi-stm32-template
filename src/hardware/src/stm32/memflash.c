#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "memflash.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <string.h>


static inline uint16_t* memflash_page_address(uint16_t* dest) {
    return (uint16_t*)(((uint32_t)dest / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE);
}

void memflash_lock(void) {
    flash_lock();
}

void memflash_unlock(void) {
    flash_unlock();
}

size_t memflash_awailable_size(void) {
    uint8_t* flash_end = (uint8_t*)memflash_end();
    uint8_t* flash_start = (uint8_t*)(APP_BASE_ADDRESS);

    return (flash_end >= flash_start) ? (size_t)(flash_end - flash_start) : 0;
}

inline uint32_t memflash_start(void) {
    return FLASH_BASE;
}

inline uint32_t memflash_end(void) {
    /* Only allow access to the chip's self-reported flash size */
    return (FLASH_BASE + DESIG_FLASH_SIZE*FLASH_PAGE_SIZE);
}

static bool memflash_write_page(uint32_t page_address, const uint8_t *data, uint16_t N)
{
    /*Erasing page*/
	flash_erase_page(page_address);
	if(flash_get_status_flags() != FLASH_SR_EOP)
		return false;

    for (uint16_t i = 0; i < N; i += 4) {
        flash_program_word(page_address + i, *((uint32_t*)(data + i)));
		// if(flash_get_status_flags() != FLASH_SR_EOP)
		// 	return false;

		// /*verify if correct data is programmed*/
		// if(*((uint32_t*)(page_address+i)) != *((uint32_t*)(data + i)))
		// 	return false;
    }

    return true;
}

bool memflash_write_block(uint8_t block_no, const uint8_t *data, uint16_t N)
{
    uint32_t page_address = memflash_end() - (block_no + 1) * FLASH_PAGE_SIZE;

    flash_unlock();
    
    bool result = memflash_write_page(page_address, data, N);

    flash_lock();

    return result;
}

/**
 * прочитать страницу флеша
 * нумерация с нуля
 * первый пишется в конец доступной памяти
 */
void memflash_read_block(uint8_t block_no, uint8_t *data, uint16_t N)
{
	uint8_t * page_ptr = (uint8_t *) (memflash_end() - (block_no + 1) * FLASH_PAGE_SIZE);

    memcpy(data, page_ptr, N);
}

bool memflash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count) {
    bool verified = true;

    /* Remember the bounds of erased data in the current page */
    static uint16_t* erase_start;
    static uint16_t* erase_end;

    const uint16_t* flash_end = (uint16_t*)memflash_end();
    // debug_print("target_flash_program_array dest "); debug_print_unsigned((size_t) dest); ////
    // debug_print(", data "); debug_print_unsigned((size_t) data); 
    // debug_print(", half_word_count "); debug_print_unsigned((size_t) half_word_count); 
    // debug_print(", flash_end "); debug_print_unsigned((size_t) flash_end); 
    // debug_println(""); debug_flush(); ////
    while (half_word_count > 0) {
        /* Avoid writing past the end of flash */
        if (dest >= flash_end) {
            //  TODO: Fails here
            // debug_println("dest >= flash_end"); debug_flush();
            verified = false;
            break;
        }

        if (dest >= erase_end || dest < erase_start) {
            erase_start = memflash_page_address(dest);
            erase_end = erase_start + (FLASH_PAGE_SIZE)/sizeof(uint16_t);
            flash_erase_page((uint32_t)erase_start);
        }
        flash_program_half_word((uint32_t)dest, *data);
        erase_start = dest + 1;
        if (*dest != *data) {
            // debug_println("*dest != *data"); debug_flush();
            verified = false;
            break;
        }
        dest++;
        data++;
        half_word_count--;
    }

    return verified;
}

#endif