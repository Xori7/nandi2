#include "nandi.h"
#include <string.h>
#include <stdio.h>

extern char *n_string_format_args(n_allocator_t *allocator, const char *format, va_list args) {
    size_t length;
    char *space;

    length = vsnprintf(NULL, 0, format, args);
    if ((space = n_memory_allocator_alloc(allocator, length + 1)) != NULL) {
        vsnprintf(space, length + 1, format, args);
        return space;
    }
    return NULL;
}

extern char *n_string_format(n_allocator_t *allocator, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *result = n_string_format_args(allocator, format, args);
    va_end(args);
    return result;
}
