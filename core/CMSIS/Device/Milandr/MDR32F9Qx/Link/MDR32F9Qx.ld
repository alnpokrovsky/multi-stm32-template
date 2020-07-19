MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
	CCMRAM(rwx): ORIGIN = 0x20000000, LENGTH = 4K
	RAM   (rwx): ORIGIN = 0x20001000, LENGTH = 28K
}

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */
/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

INCLUDE core/CMSIS/cortex-m-generic.ld
