#ifndef NANDI_H
#define NANDI_H

#ifdef TEST_BUILD
#include "../config.h"
#else
#include "../build/config.h"
#endif // TEST_BUILD

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

typedef void *(*n_alloc_tfn)(const n_allocator_t *allocator, size_t size);
typedef void *(*n_realloc_tfn)(const n_allocator_t *allocator, void *ptr, size_t size);
typedef void (*n_free_tfn)(const n_allocator_t *allocator, void *ptr);

struct i_n_allocator_t {
    n_alloc_tfn alloc_fn;
    n_realloc_tfn realloc_fn;
    n_free_tfn free_fn;
};

void *n_memory_allocator_alloc(const n_allocator_t *allocator, size_t size);
void *n_memory_allocator_realloc(const n_allocator_t *allocator, void *ptr, size_t size);
void n_memory_allocator_free(const n_allocator_t *allocator, void *ptr);

const n_allocator_t *n_memory_get_default_allocator(void);

/*************************
Module: NString
Source: nstring.c
*************************/
extern char *n_string_format_args(const n_allocator_t *allocator, const char *format, va_list args);
extern char *n_string_format(const n_allocator_t *allocator, const char *format, ...);

/*************************
Module: NThreading
Source: nthreading_windows.c
*************************/
typedef void *n_thread_t;
typedef void *n_mutex_t;

extern n_thread_t   n_threading_thread_create(void (*func)(void*), void* args); // Creates new thread and executes func with args
extern void         n_threading_thread_wait(n_thread_t thread); //Waits until thread finishes its execution
extern void         n_threading_thread_terminate(n_thread_t thread, int exitCode); // Terminates thread with exitCode
extern void         n_threading_thread_sleep(uint64_t milliseconds);
extern n_thread_t   n_threading_get_current_thread(void);
extern uint64_t     n_threading_get_current_thread_id(void);

extern n_mutex_t    n_threading_mutex_create(void); // Creates a mutex
extern bool_t       n_threading_mutex_wait(n_mutex_t mutex); // Waits until mutex is unlocked and locks it for current thread
extern bool_t       n_threading_mutex_release(n_mutex_t mutex); // Releases mutex lock state

/*************************
Module: NLog
Source: nlog.c
*************************/
#define ANSI_COLOR_RED     "\x1b[91m"
#define ANSI_COLOR_GREEN   "\x1b[92m"
#define ANSI_COLOR_YELLOW  "\x1b[93m"
#define ANSI_COLOR_BLUE    "\x1b[94m"
#define ANSI_COLOR_MAGENTA "\x1b[95m"
#define ANSI_COLOR_CYAN    "\x1b[96m"
#define ANSI_COLOR_WHITE   "\x1b[97m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_TEST,
    LL_COUNT
} n_log_level_enum;

static const char *N_LOG_LEVEL_NAMES[] = {
    "INFO",
    "WARN",
    "ERRO",
    "TEST"
};

typedef struct {
    uint16_t level;
    uint16_t category;
    time_t time;
    uint64_t thread_id;
    char *message;
} n_log_data_t;

typedef void (*n_log_handler_tfn)(n_log_data_t logData, void *customData);

typedef struct {
    n_log_handler_tfn handler_fn;
    void *custom_data;
} n_log_handler_t;

#define N_LOG_MAX_HANDLERS_COUNT 16

extern uint32_t n_log_register_console_handler(void);
extern uint32_t n_log_register_file_handler(const char *file);
extern uint32_t n_log_register_handler(n_log_handler_t handler);
extern void n_log_unregister_handler(n_log_handler_t handler);
extern void n_log(uint16_t level, uint16_t category, char *message);
extern void n_log_format(const n_allocator_t *allocator, uint16_t level, uint16_t category, char *format, ...);

/*************************
Module: NTest
Source: ntest.c
*************************/
void i_n_test_assert(const char *testName, int32_t testLine, bool_t condition, const char *format1, const char *format2, ...);

#define n_assert_true(value) i_n_test_assert(__FILE__, __LINE__, value, "%s", "%s", "true", "false")
#define n_assert_false(value) i_n_test_assert(__FILE__, __LINE__, !value, "%s", "%s", "false", "true")

#define i_n_assert_compare(exp, act, condition, format1, format2) i_n_test_assert(__FILE__, __LINE__, act condition exp, format1, format2, exp, act)
#define n_assert_size_eq(exp, act)      i_n_assert_compare(exp, act, ==,  "%z",  "%z")
#define n_assert_size_greater(exp, act) i_n_assert_compare(exp, act, >, "> %z",  "%z")
#define n_assert_size_lower(exp, act)   i_n_assert_compare(exp, act, <, "< %z",  "%z")
#define n_assert_i32_eq(exp, act)       i_n_assert_compare(exp, act, ==,  "%i",  "%i")
#define n_assert_i32_greater(exp, act)  i_n_assert_compare(exp, act, >, "> %i",  "%i")
#define n_assert_i32_lower(exp, act)    i_n_assert_compare(exp, act, <, "< %i",  "%i")
#define n_assert_u32_eq(exp, act)       i_n_assert_compare(exp, act, ==,  "%u",  "%u")
#define n_assert_u32_greater(exp, act)  i_n_assert_compare(exp, act, >, "> %u",  "%u")
#define n_assert_u32_lower(exp, act)    i_n_assert_compare(exp, act, <, "< %u",  "%u")
#define n_assert_i64_eq(exp, act)       i_n_assert_compare(exp, act, ==,  "%li", "%li")
#define n_assert_i64_greater(exp, act)  i_n_assert_compare(exp, act, >, "> %li", "%li")
#define n_assert_i64_lower(exp, act)    i_n_assert_compare(exp, act, <, "< %li", "%li")
#define n_assert_u64_eq(exp, act)       i_n_assert_compare(exp, act, ==,  "%lu", "%lu")
#define n_assert_u64_greater(exp, act)  i_n_assert_compare(exp, act, >, "> %lu", "%lu")
#define n_assert_u64_lower(exp, act)    i_n_assert_compare(exp, act, <, "< %lu", "%lu")

#endif //NANDI_H

// vi: ft=c

