#ifndef __UTILS_DELAY_H__
#define __UTILS_DELAY_H__


void delay_some(void);

void delay_a_bit(void);

#define DELAY_TILL(flag)  for(int delayedtilli = 0; delayedtilli < 10000; ++delayedtilli) { if ((flag)) break; }

#endif
