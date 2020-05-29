#ifndef __UTILS_DELAY_H__
#define __UTILS_DELAY_H__


void delay_some(void);

void delay_a_bit(void);

#define DELAY_TILL(flag) DELAY_TILL_4(flag)

#define DELAY_TILL_4(flag) \
    { for(int delayedtilli = 0; delayedtilli < 10000; ++delayedtilli) {if ((flag)) break; } }
#define DELAY_TILL_5(flag) \
    { for(int delayedtilli = 0; delayedtilli < 100000; ++delayedtilli) {if ((flag)) break; } }

#endif
