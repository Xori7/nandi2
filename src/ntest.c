#ifndef TEST_BUILD
#include "nandi.h"
#include <stdarg.h>

void i_n_test_assert(const char *testName, int32_t testLine, bool_t condition, const char *format1, const char *format2, ...) {
    const n_allocator_t *allocator = n_memory_get_default_allocator();
    if (condition == 1) {
        n_log_format(allocator, LL_TEST, 0, "%s:%d has passed\n", testName, testLine);
    } else {
        char *detailsFormat = n_string_format(allocator, "\n\texpected: %s \n\tactual: %s", format1, format2);
        va_list args;
        va_start(args, format2);
        char *detailsText = n_string_format_args(allocator, detailsFormat, args);
        va_end(args);
        n_log_format(allocator, LL_TEST, 0, "%s:%d has failed %s\n", testName, testLine, detailsText);
        n_memory_allocator_free(allocator, detailsFormat);
        n_memory_allocator_free(allocator, detailsText);
    }
}
#endif // TEST_BUILD
