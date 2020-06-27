/* Define memory regions. */
MEMORY
{
	rom  (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
 	itcm(rwx) : ORIGIN = 0x00000000, LENGTH = 16K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 320K
}

/* my improved linker with sramfunc section */
INCLUDE make/stm32/cortex-m-generic-s.ld
