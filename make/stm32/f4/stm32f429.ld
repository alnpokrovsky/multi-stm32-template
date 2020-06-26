/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
 	itcm (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20010000, LENGTH = 128K
    ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

/* important!!! you cannot use ccm as instruction
 * memory in stm32f4
 */
SECTIONS {
    .sram_func : ALIGN(4) {
        __sram_func = .;
        *(.sram_func*)
        . = ALIGN(4);
        __esram_func = .;
    } >itcm AT >rom
    __sram_func_loadaddr = LOADADDR(.sram_func);
}
