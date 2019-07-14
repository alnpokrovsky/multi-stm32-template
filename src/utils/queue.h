/**
 * my cyclic queue released for C
 * you have to define QUEUE_TYPE and QUEUE_SIZE to use it
 * QUEUE_TYPE better to be array (but it's not accurate)
 * also don't recomend you to use this in header files
 */

#ifdef QUEUE_TYPE
#ifdef QUEUE_SIZE

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

static struct {
    QUEUE_TYPE q[QUEUE_SIZE];
    size_t size;
    size_t head;
} _queue = {.size = 0, .head = 0};

static inline bool queue_push(const QUEUE_TYPE * el) {
    if (_queue.size >= QUEUE_SIZE) return false;
    
    void * dst = &_queue.q[(_queue.head+_queue.size) % QUEUE_SIZE];
    memcpy(dst, el, sizeof(QUEUE_TYPE));
    ++_queue.size;
    return true;
}

/**
 * don't forget to check if size == 0
 */
static inline QUEUE_TYPE queue_pop(void) {
    QUEUE_TYPE result = _queue.q[_queue.head];
    --_queue.size;
    _queue.head = (_queue.head+1) % QUEUE_SIZE;
    return result;
}

static inline size_t queue_size(void) {
    return _queue.size;
}

#endif
#endif