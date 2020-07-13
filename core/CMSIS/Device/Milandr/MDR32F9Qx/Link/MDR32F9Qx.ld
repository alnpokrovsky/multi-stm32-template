/* Highest address of the user mode stack */
_estack = 0x20050000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x500;      /* required amount of heap  */
_Min_Stack_Size = 0x500; /* required amount of stack */

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
	CCMRAM(rwx): ORIGIN = 0x20000000, LENGTH = 4K
	RAM   (rwx): ORIGIN = 0x20001000, LENGTH = 28K
}

INCLUDE core/CMSIS/cortex-m-generic.ld
