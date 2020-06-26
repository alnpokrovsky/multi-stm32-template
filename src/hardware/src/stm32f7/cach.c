#if defined(STM32F7)

#include <stdint.h>

// Writing to the ICIALLU completely invalidates the entire instruction cache
#define ICIALLU (*(volatile uint32_t *)(0xE000EF50))
// The Configuration and Control Register contains the control bits for
// enabling/disabling caches
#define ARM_CCR (*(volatile uint32_t *)(0xE000ED14))

// We will also define two macros for data and instruction barriers
#define __dsb __asm__ __volatile__("dsb" ::: "memory")
#define __isb __asm__ __volatile__("isb" ::: "memory")


static void __attribute__ ((constructor)) premainInitICach(void) {
    // Synchronize
    __dsb; __isb;
    // Invalidate the instruction cache
    ICIALLU = 0UL;
    // Re-synchronize
    __dsb; __isb;
    // Enable the I-cache
    ARM_CCR |= (1 << 17);
    // Force a final resync and clear of the instruction pipeline
    __dsb; __isb;
}

#endif
