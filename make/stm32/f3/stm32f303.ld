/* Linker script for ST STM32F3DISCOVERY (STM32F303VC, 256K flash, 48K RAM (Of which 8kB are CCM RAM and 40KB are SRAM). */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 256K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 40K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld
