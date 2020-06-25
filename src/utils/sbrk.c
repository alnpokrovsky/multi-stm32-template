/*
 * sbrk.c
 *
 * from https://github.com/libopencm3/libopencm3/wiki/Using-malloc-with-libopencm3
 * 
 * This code written July 2018 by Chuck McManis (cmcmanis@mcmanis.com)
 * it may be freely used, reused, and modified with our without accrediting the
 * author.
 * 
 * This implements a larger heap than you get by default
 * in newlib. It uses the RAM between the end of the declared
 * data and the bottom of the "largest" stack for the heap.
 *
 * The only user interface to this code is the weakly bound
 * local_heap_setup() which is called the first time malloc
 * has to get more memory for the heap. This function passes
 * pointers to its two controls, the start address and end
 * address of the heap. Once called those are not changed.
 *
 * If the user does not define local_heap_setup in their code,
 * the RAM between EBSS and HEAP_END - MAXHEAP_END_SIZE is used
 * for the heap. Typically this is RAM that is internal to the
 * SoC.
 *
 * On other processors, additional ram might become available
 * after reset and some peripheral setup (like PSRAM or SDRAM
 * on parts with the FMC peripheral). In that case the user
 * can define their own function local_heap_setup and put
 * the start and end address of heap into that external RAM.
 *
 * Note that memory MUST be contiguous between the start and
 * end point of the heap. If there are gaps in that memory
 * then malloc() will hard fault when it tries to use
 * memory from the gap.
 */
#include <stdint.h>
#include <errno.h>
#include <malloc.h>

/**
 *     ...
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

#if defined(STM32F1) || defined(STM32F3)
    extern uint8_t _ebss, _stack;/* these are defined by the linker script */
    #define INIT_MEM()
    /* reserve half for stack and half for heap */
    #define HEAP_START &_ebss
    #define HEAP_END &_stack - (&_stack - &_ebss)/2
#elif defined(STM32F4) | defined(STM32F7)
    #include "sdram.h"
    #define INIT_MEM() sdram_init()
    #define HEAP_START _sdram
    #define HEAP_END _esdram
#elif defined(USE_MDR1986VE9x)
    extern uint8_t __bss_end__, __stack;
    #define INIT_MEM()
    #define HEAP_START &__bss_end__
    #define HEAP_END &__stack - (&__stack - &__bss_end__)/2
#endif

/** this function once called when first malloc
 * it will not be called if there are no malloc usings
 */
void local_heap_setup(uint8_t **start, uint8_t **end);
void local_heap_setup(uint8_t **start, uint8_t **end)
{
    INIT_MEM();
    *start = HEAP_START;
    *end = HEAP_END;
}

void * _sbrk_r(struct _reent *reent, ptrdiff_t diff);
void * _sbrk_r(struct _reent *reent, ptrdiff_t diff)
{
    static uint8_t *_cur_brk = NULL;
    static uint8_t *_heap_end = NULL;

    uint8_t *_old_brk;

    if (_heap_end == NULL) {
        local_heap_setup(&_cur_brk, &_heap_end);
    }

    _old_brk = _cur_brk;
    if (_cur_brk + diff > _heap_end) {
        reent->_errno = ENOMEM;
        return (void *)-1;
    }
    _cur_brk += diff;
    return _old_brk;
}
