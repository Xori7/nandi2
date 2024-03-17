#ifndef TEST_BUILD
#include "nandi.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

n_log_handler_t handlers[N_LOG_MAX_HANDLERS_COUNT] = { 0 };

static void i_n_log_console_handler(n_log_data_t logData, void *customData) {
    printf("[%s][%d] %s", N_LOG_LEVEL_NAMES[logData.level], logData.category, logData.message);
}

static void i_n_log_file_handler(n_log_data_t logData, void *customData) {
    //printf("[%s][%d]", N_LOG_LEVEL_NAMES[logData.level], logData.category);
}

//TODO: Maybe change that to n_log_get_default_console_handler() and than use it (f.ex. register)
extern uint32_t n_log_register_console_handler(void) {
    return n_log_register_handler((n_log_handler_t){ .handler_fn = &i_n_log_console_handler, .custom_data = NULL });
}

extern uint32_t n_log_register_file_handler(const char *file) {
    return n_log_register_handler((n_log_handler_t){ .handler_fn = &i_n_log_file_handler, .custom_data = (void*)file });
}

extern uint32_t n_log_register_handler(n_log_handler_t handler) {
    for (uint32_t i = 0; i < N_LOG_MAX_HANDLERS_COUNT; i++) {
        if (handlers[i].handler_fn == NULL) {
            handlers[i] = handler;
            return 0;
        }
    }
    return -1;
}

extern void n_log_unregister_handler(n_log_handler_t handler) {
    for (uint32_t i = 0; i < N_LOG_MAX_HANDLERS_COUNT; i++) {
        if (handlers[i].handler_fn == handler.handler_fn) {
            handlers[i] = (n_log_handler_t){ 0 };
            return;
        }
    }
}

static void handlers_invoke(n_log_data_t logData) {
    for (uint32_t i = 0; i < N_LOG_MAX_HANDLERS_COUNT; i++) {
        if (handlers[i].handler_fn != NULL) {
            (handlers[i].handler_fn)(logData, handlers[i].custom_data);
        }
    }
}

extern void n_log(uint16_t level, uint16_t category, char *message) {
    n_log_data_t log_data = {
        .level = level,
        .category = category,
        .time = time(0),
        .message = message
    };
    handlers_invoke(log_data);
}

extern void n_log_format(const n_allocator_t *allocator, uint16_t level, uint16_t category, char *format, ...) {
    va_list args;
    va_start(args, format);
    char *message = n_string_format_args(allocator, format, args);
    va_end(args);
    n_log(level, category, message);
    n_memory_allocator_free(allocator, message);
}

#endif // TEST_BUILD
