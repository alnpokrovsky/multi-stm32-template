#ifndef __HARDWARE_DIGITALPIN_H__
#define __HARDWARE_DIGITALPIN_H__

#include <stdint.h>

typedef enum {
    DIGITALPIN_INPUT,
    DIGITALPIN_OUTPUT,
} DIGITALPIN_MODE;

typedef enum {
    PA_0 ,  PA_1 ,  PA_2 ,  PA_3 ,  PA_4 ,  PA_5 ,  PA_6 ,  PA_7 ,  PA_8 ,  PA_9 ,  PA_10,  PA_11,  PA_12,  PA_13,  PA_14,  PA_15,
    PB_0 ,  PB_1 ,  PB_2 ,  PB_3 ,  PB_4 ,  PB_5 ,  PB_6 ,  PB_7 ,  PB_8 ,  PB_9 ,  PB_10,  PB_11,  PB_12,  PB_13,  PB_14,  PB_15,
    PC_0 ,  PC_1 ,  PC_2 ,  PC_3 ,  PC_4 ,  PC_5 ,  PC_6 ,  PC_7 ,  PC_8 ,  PC_9 ,  PC_10,  PC_11,  PC_12,  PC_13,  PC_14,  PC_15,
    PD_0 ,  PD_1 ,  PD_2 ,  PD_3 ,  PD_4 ,  PD_5 ,  PD_6 ,  PD_7 ,  PD_8 ,  PD_9 ,  PD_10,  PD_11,  PD_12,  PD_13,  PD_14,  PD_15,
} DIGITALPIN_NAME;

/**
 * set input(pull down) or output(pushpull) mode for pin
 * and activate its periph rcc
 */
void digitalpin_mode(DIGITALPIN_NAME pn, DIGITALPIN_MODE pmode);

/**
 * set pin state (bool)
 */
void digitalpin_set(DIGITALPIN_NAME pn, uint16_t state);

/**
 * toggle pin state
 */
void digitalpin_toggle(DIGITALPIN_NAME pn);

/**
 * get pin state (bool)
 */
uint16_t digitalpin_get(DIGITALPIN_NAME pn);

#endif
