#ifndef __UTILS_WORD_H__
#define __UTILS_WORD_H__

#include <stdint.h>

#pragma pack(push, 1)
typedef union {
    struct {
        uint8_t byte_low;     // low order byte
        uint8_t byte_high;    // high order byte
    };
    uint16_t word;            // 16 bits presentation
} WORD2BYTES;
#pragma pack(pop)

uint16_t word_swap(uint16_t w);

uint16_t word_from_bytes(uint8_t * bytes, uint16_t pos);

void word_to_bytes(uint16_t w, uint8_t * bytes, uint16_t pos);

#endif
