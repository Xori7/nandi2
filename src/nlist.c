#ifndef TEST_BUILD
#include "nandi.h"

typedef struct {
    const n_allocator_t *allocator;
    size_t element_size;
    uint32_t capacity;
    uint32_t length;
    uint8_t buffer[1];
} i_n_list_wrapped_t;

static i_n_list_wrapped_t *i_n_list_wrap(void *list) {
    i_n_list_wrapped_t *p = (i_n_list_wrapped_t*)(list - (void*)&((i_n_list_wrapped_t*)0)->buffer);
    return p;
}

void i_n_list_set_length(void *list, uint32_t length) {
    i_n_list_wrapped_t *wrapped = i_n_list_wrap(list);
    wrapped->length = length;
    if (length > wrapped->capacity) {
        wrapped->capacity *= 2;
        n_memory_allocator_realloc(wrapped->allocator, wrapped, sizeof(i_n_list_wrapped_t) + wrapped->capacity * wrapped->element_size);
    }
}

extern void *n_list_create(const n_allocator_t *allocator, size_t elementSize, uint32_t capacity) {
    i_n_list_wrapped_t *list = n_memory_allocator_alloc(allocator, sizeof(i_n_list_wrapped_t) + capacity * elementSize);
    list->allocator = allocator;
    list->element_size = elementSize;
    list->capacity = capacity;
    list->length = 0;
    return list->buffer;
}


extern void n_list_clear(void *list) {
    i_n_list_wrapped_t *wrapped = i_n_list_wrap(list);
    wrapped->length = 0;
}

extern uint32_t n_list_length(void *list) {
    return i_n_list_wrap(list)->length;
}

extern uint32_t n_list_capacity(void *list) {
    return i_n_list_wrap(list)->capacity;
}

// extern void n_list_sort(void *list);
// extern void n_list_trim_excess(void *list);
// extern void n_list_destroy(void *list);
#endif // TEST_BUILD

#ifdef TEST_IMPL
TEST("New list length equals 0") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_assert_u32_eq(0, n_list_length(list));
}

TEST("After add length increases by 1") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_assert_u32_eq(0, n_list_length(list));
    n_list_add(list, 12);
    n_assert_u32_eq(1, n_list_length(list));
    n_list_add(list, 10002341);
    n_assert_u32_eq(2, n_list_length(list));
}

TEST("After add values in the list are correct") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_list_add(list, 12);
    n_list_add(list, 10002341);
    n_assert_u32_eq(12, list[0]);
    n_assert_u32_eq(10002341, list[1]);
}

TEST("After more then capacity adds capacity doubles") {
    uint32_t capacity = 4;
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), capacity);
    for (uint32_t i = 0; i < capacity + 1; i++) {
        n_list_add(list, 7);
    }
    n_assert_u32_eq(capacity * 2, n_list_capacity(list));
}

TEST("add_at sets correct element values") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_list_add(list, 0);
    n_list_add(list, 1);
    n_list_add(list, 3);
    n_list_add(list, 4);
    n_list_add_at(list, 2, 2);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(1, list[1]);
    n_assert_u32_eq(2, list[2]);
    n_assert_u32_eq(3, list[3]);
    n_assert_u32_eq(4, list[4]);
}

TEST("remove_at sets correct element values") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 4);
    n_list_add(list, 0);
    n_list_add(list, 3);
    n_list_add(list, 1);
    n_list_add(list, 2);
    n_list_remove_at(list, 1);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(2, list[1]);
    n_assert_u32_eq(1, list[2]);
}

TEST("remove_at_ordered sets correct element values") {
    LIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 4);
    n_list_add(list, 0);
    n_list_add(list, 3);
    n_list_add(list, 1);
    n_list_add(list, 2);
    n_list_remove_at_ordered(list, 1);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(1, list[1]);
    n_assert_u32_eq(2, list[2]);
}

#endif // TEST_IMPL

