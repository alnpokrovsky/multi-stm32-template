#if defined(STM32F3)

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


extern uint8_t _ebss, _stack;/* these are defined by the linker script */
/* reserve half for stack and half for heap */
#define HEAP_START &_ebss
#define HEAP_END &_stack - (&_stack - &_ebss)/2


/** this function once called when first malloc
 * it will not be called if there are no malloc usings
 */
static void local_heap_setup(uint8_t **start, uint8_t **end)
{
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

#endif
