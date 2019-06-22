#ifndef __UTILS_BYTE_H__
#define __UTILS_BYTE_H__

#include <stdint.h>

typedef uint8_t byte;
typedef uint16_t word;

static inline byte HIGHBYTE(word w) {
    return (byte) ((w >> 8) & 0xFF);
}

static inline byte LOWBYTE(word w) {
    return (byte) ((w >> 0) & 0xFF);
}

static inline word BYTE2WORD(byte bH, byte bL) {
    return ((word) (bH) << 8) | (word) (bL);
}

#endif
