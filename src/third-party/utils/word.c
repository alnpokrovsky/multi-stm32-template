#include "word.h"

uint16_t word_swap(uint16_t w) {
    WORD2BYTES result;
    uint8_t tmp;
    result.word = w;

    tmp = result.byte_high;
    result.byte_high = result.byte_low;
    result.byte_low = tmp;

    return result.word;
}

uint16_t word_from_bytes(uint8_t * bytes, uint16_t pos) {
    uint16_t * words = (uint16_t *) bytes;
    return words[pos];
}

void word_to_bytes(uint16_t w, uint8_t * bytes, uint16_t pos) {
    uint16_t * words = (uint16_t *) bytes;
    words[pos] = w;
}
