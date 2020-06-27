#include "sramfunc.h"

static const unsigned NULL_PTR = 0;

#pragma weak __sram_func = NULL_PTR
#pragma weak __esram_func = NULL_PTR
#pragma weak __sram_func_loadaddr = NULL_PTR

extern unsigned __sram_func, __esram_func, __sram_func_loadaddr;

/**
 * function executed before main and initialize ram
 * with fuctions that must start from ram
*/
static void __attribute__ ((constructor)) premainInitFuncs(void) {
	volatile unsigned *src, *dest;

	for (src = &__sram_func_loadaddr, dest = &__sram_func;
		dest < &__esram_func;
		src++, dest++) {
		    *dest = *src;
	}
}
