#ifndef NANDI_H
#define NANDI_H

#include "../build/config.h"

/*************************
Primitve types
*************************/
#include <stdint.h>

/*************************
Module: NMemory
Source: nmemory.c
*************************/
typedef struct i_n_allocator_t n_allocator_t;

typedef void *(*n_alloc_tfn)(n_allocator_t *allocator, size_t size);
typedef void *(*n_realloc_tfn)(n_allocator_t *allocator, void *ptr, size_t size);
typedef void (*n_free_tfn)(n_allocator_t *allocator, void *ptr);

struct i_n_allocator_t {
    n_alloc_tfn alloc_fn;
    n_realloc_tfn realloc_fn;
    n_free_tfn free_fn;
};

void *n_memory_allocator_alloc(n_allocator_t *allocator, size_t size);
void *n_memory_allocator_realloc(n_allocator_t *allocator, void *ptr, size_t size);
void n_memory_allocator_free(n_allocator_t *allocator, void *ptr);

const n_allocator_t *n_memory_default_allocator_get();

/*************************
Module: NString
Source: nstring.c
*************************/
typedef char* n_string_t;

n_string_t n_string_from_cstring(n_allocator_t *allocator, const char *cstring);
void n_string_dispose(n_string_t string);

/*************************/

#endif //NANDI_H

// vi: ft=c
