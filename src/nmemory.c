#ifndef TEST_BUILD
#include "nandi.h"

typedef struct {
    n_alloc_tfn alloc_fn;
    n_realloc_tfn realloc_fn;
    n_free_tfn free_fn;
    void *custom_data;
} i_n_allocator_t;

/*default_allocator*/
#include <stdlib.h>
void *i_default_allocator_alloc(n_allocator_t allocator, size_t size) {
    return malloc(size);
}
void *i_default_allocator_realloc(n_allocator_t allocator, void *ptr, size_t size) {
    return realloc(ptr, size);
}
void i_default_allocator_free(n_allocator_t allocator, void *ptr) {
    return free(ptr);
}
const i_n_allocator_t I_DEFAULT_ALLOCATOR = {
    .alloc_fn = &i_default_allocator_alloc,
    .realloc_fn = &i_default_allocator_realloc,
    .free_fn = &i_default_allocator_free,
    .custom_data = NULL,
};
n_allocator_t n_memory_get_default_allocator(void) {
    return (n_allocator_t)&I_DEFAULT_ALLOCATOR;
}
// default_allocator

/*allocator*/
extern void *n_memory_allocator_alloc(n_allocator_t allocator, size_t size) {
    return (((i_n_allocator_t*)allocator)->alloc_fn)(allocator, size);
}
extern void *n_memory_allocator_realloc(n_allocator_t allocator, void *ptr, size_t size) {
    return (((i_n_allocator_t*)allocator)->realloc_fn)(allocator, ptr, size);
}
extern void n_memory_allocator_free(n_allocator_t allocator, void *ptr) {
    return (((i_n_allocator_t*)allocator)->free_fn)(allocator, ptr);
}
// allocator

#endif // TEST_BUILD
