/* Define memory regions. */
MEMORY
{
	rom  (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
 	itcm(rwx) : ORIGIN = 0x00000000, LENGTH = 16K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 320K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

SECTIONS {
    .sram_func : ALIGN(4) {
        __sram_func = .;
        *(.sram_func*)
        . = ALIGN(4);
        __esram_func = .;
    } >ram AT >itcm
    __sram_func_loadaddr = LOADADDR(.sram_func);
}
