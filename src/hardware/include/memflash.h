#ifndef __HARDWARE_MEMFLASH_H__
#define __HARDWARE_MEMFLASH_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void memflash_lock(void);

void memflash_unlock(void);

uint32_t memflash_end(void);

size_t memflash_awailable_size(void);

bool memflash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count);

#endif