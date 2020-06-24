#ifndef _UTILS_DATA32_H_
#define _UTILS_DATA32_H_

#include <stdint.h>

#pragma pack(push,1)
typedef union {
    float f;
    int32_t i;
    uint32_t u;
    uint8_t b[4];
} Data32;
#pragma pack(pop)

#endif
