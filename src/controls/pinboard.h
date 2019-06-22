#ifndef __CONTROLS_MATRIXKEYBOARD_H__
#define __CONTROLS_MATRIXKEYBOARD_H__

#include <stdint.h>

#define COLS_N 8
#define ROWS_N 10

/**
 * init used GPIO ports 
 */
void pinboard_init(void);

/**
 * get pressed key in current row and go to next
 */
uint8_t pinboard_next(void);

/**
 * get pressed modifier keys (CTRL in this case)
 */
uint8_t pinboard_modifiers(void);

#endif
