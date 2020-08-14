/* Specify the memory areas */
MEMORY 
{
    FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    RAM     (xrw) : ORIGIN = 0x20000000, LENGTH = 320K
    CCMRAM  (xrw) : ORIGIN = 0x00000000, LENGTH = 16K
}

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size =  1/4 * LENGTH(RAM); /* required amount of heap  */
_Min_Stack_Size = 3/4 * LENGTH(RAM); /* required amount of stack */

INCLUDE core/CMSIS/cortex-m-generic.ld
