#ifndef __UTILS_DELAY_H__
#define __UTILS_DELAY_H__

#include <stdbool.h>

void delay_ms(int ms);

#define DELAY_TILL(flag) DELAY_TILL_CYCLES(flag, 100000)

#define DELAY_TILL_CYCLES(flag, cycles) ({ \
    bool result; \
    {for(int delay_till_i = 0; delay_till_i < cycles; ++delay_till_i) \
        if ((result = flag)) break;} \
    result; })

#endif
