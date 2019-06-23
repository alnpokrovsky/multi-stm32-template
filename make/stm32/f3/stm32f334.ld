
/* Specify the memory areas */
MEMORY
{
    rom (rx): ORIGIN = 0x8000000, LENGTH = 64K
    ram (xrw): ORIGIN = 0x20000000, LENGTH = 12K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld
