#ifndef __CONTROLS_STMPE811_H__
#define __CONTROLS_STMPE811_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t z;
} STMPE811_Point;

bool stmpe811_init(void);

STMPE811_Point stmpe811_read(void);

#endif
