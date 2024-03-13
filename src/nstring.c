#include "nandi.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t length;
    n_allocator_t *allocator;
    char buffer[1];
} i_n_string_wrapped_t;

i_n_string_wrapped_t *n_string_wrapped_new(n_allocator_t *allocator, uint32_t length) {
    i_n_string_wrapped_t *wrapped = n_memory_allocator_alloc(allocator, sizeof(*wrapped) + length);
    wrapped->length = length;
    wrapped->allocator = allocator;
    wrapped->buffer[length] = 0;
    return wrapped;
}

n_string_t n_string_from_cstring(n_allocator_t *allocator, const char *cstring) {
    i_n_string_wrapped_t *wrapped;
    wrapped = n_string_wrapped_new(allocator, strlen(cstring));
    strcpy(wrapped->buffer, cstring);
    return (n_string_t)wrapped->buffer;
}

void n_string_dispose(n_string_t string) {
    i_n_string_wrapped_t *wrapped = (i_n_string_wrapped_t*)(string - ((i_n_string_wrapped_t*)0)->buffer);
    n_memory_allocator_free(wrapped->allocator, wrapped);
}
