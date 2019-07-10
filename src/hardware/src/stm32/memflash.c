#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "memflash.h"
#include "DeviceConfig.h"
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>


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
    return (FLASH_BASE + (size_t)DESIG_FLASH_SIZE*FLASH_PAGE_SIZE);
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