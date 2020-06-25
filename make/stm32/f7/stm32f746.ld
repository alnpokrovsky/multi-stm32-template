/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
 	ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 16K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 320K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld
