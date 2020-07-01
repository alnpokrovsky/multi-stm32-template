#if defined(STM32F7)

#include <stdint.h>
#include <errno.h>
#include <malloc.h>
#include "sdram.h"


#define INIT_MEM() sdram_init()
#define HEAP_START _sdram
#define HEAP_END _esdram

/** this function once called when first malloc
 * it will not be called if there are no malloc usings
 */
static void local_heap_setup(uint8_t **start, uint8_t **end)
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

#endif
