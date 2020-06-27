/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
 	itcm (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20010000, LENGTH = 128K
    ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

/*INCLUDE cortex-m-generic.ld*/
/* my improved linker with sramfunc section */
INCLUDE make/stm32/cortex-m-generic-s.ld
