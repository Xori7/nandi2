#ifndef TEST_BUILD
#include "nandi.h"

/*allocator*/
void *n_memory_allocator_alloc(const n_allocator_t *allocator, size_t size) {
    return (allocator->alloc_fn)(allocator, size);
}
void *n_memory_allocator_realloc(const n_allocator_t *allocator, void *ptr, size_t size) {
    return (allocator->realloc_fn)(allocator, ptr, size);
}
void n_memory_allocator_free(const n_allocator_t *allocator, void *ptr) {
    return (allocator->free_fn)(allocator, ptr);
}
// allocator

/*default_allocator*/
#include <stdlib.h>
void *i_default_allocator_alloc(const n_allocator_t *allocator, size_t size) {
    return malloc(size);
}
void *i_default_allocator_realloc(const n_allocator_t *allocator, void *ptr, size_t size) {
    return realloc(ptr, size);
}
void i_default_allocator_free(const n_allocator_t *allocator, void *ptr) {
    return free(ptr);
}

const n_allocator_t I_DEFAULT_ALLOCATOR = {
    .alloc_fn = &i_default_allocator_alloc,
    .realloc_fn = &i_default_allocator_realloc,
    .free_fn = &i_default_allocator_free,
};

const n_allocator_t *n_memory_get_default_allocator(void) {
    return &I_DEFAULT_ALLOCATOR;
}
// default_allocator

#endif // TEST_BUILD
