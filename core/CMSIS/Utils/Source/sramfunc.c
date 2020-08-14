#include "sramfunc.h"


extern unsigned _sccmram, _eccmram, _ccmram_loadaddr;

/**
 * function executed before main and initialize ram
 * with fuctions that must start from ram
*/
static void __attribute__ ((constructor)) premain_initCcmRam(void) {
	volatile unsigned *src, *dest;

	for (src = &_ccmram_loadaddr, dest = &_sccmram;
		dest < &_eccmram;
		src++, dest++) {
		    *dest = *src;
	}
}
