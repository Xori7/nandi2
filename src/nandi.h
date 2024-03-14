#ifndef NANDI_H
#define NANDI_H

#include "../build/config.h"
#include <stdlib.h>
#include <stdarg.h>

/*************************
Primitve types
*************************/
#include <stdint.h>
typedef uint8_t bool_t;

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
extern char *n_string_format_args(n_allocator_t *allocator, const char *format, va_list args);
extern char *n_string_format(n_allocator_t *allocator, const char *format, ...);

/*************************
Module: n_thread_ting
Source: nthreading_windows.c
*************************/
typedef void *n_thread_t;
typedef void *n_mutex_t;

extern n_thread_t   n_threading_thread_create(void (*func)(void*), void* args); // Creates new thread and executes func with args
extern void         n_threading_thread_wait(n_thread_t thread); //Waits until thread finishes its execution
extern void         n_threading_thread_terminate(n_thread_t thread, int exitCode); // Terminates thread with exitCode
extern void         n_threading_thread_sleep(uint64_t milliseconds);
extern n_thread_t   n_threading_get_current_thread();
extern uint64_t     n_threading_get_current_thread_id();

extern n_mutex_t    n_threading_mutex_create(); // Creates a mutex
extern bool_t       n_threading_mutex_wait(n_mutex_t mutex); // Waits until mutex is unlocked and locks it for current thread
extern bool_t       n_threading_mutex_release(n_mutex_t mutex); // Releases mutex lock state

// Logger
#define ANSI_COLOR_RED     "\x1b[91m"
#define ANSI_COLOR_GREEN   "\x1b[92m"
#define ANSI_COLOR_YELLOW  "\x1b[93m"
#define ANSI_COLOR_BLUE    "\x1b[94m"
#define ANSI_COLOR_MAGENTA "\x1b[95m"
#define ANSI_COLOR_CYAN    "\x1b[96m"
#define ANSI_COLOR_WHITE   "\x1b[97m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#endif //NANDI_H

// vi: ft=c
