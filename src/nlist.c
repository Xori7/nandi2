#ifndef TEST_BUILD
#include "nandi.h"
#include "string.h"

typedef struct {
    n_allocator_t allocator;
    size_t element_size;
    uint32_t capacity;
    uint32_t length;
} i_n_list_wrapped_t;

static i_n_list_wrapped_t *i_wrap(void *list) {
    i_n_list_wrapped_t *p = &((i_n_list_wrapped_t*)list)[-1];
    return p;
}

static void i_ensure_capacity(void **list, uint32_t length) {
    i_n_list_wrapped_t *wrapped = i_wrap(*list);
    wrapped->length = length;
    if (length > wrapped->capacity) {
        wrapped->capacity *= 2;
        wrapped = n_memory_allocator_realloc(wrapped->allocator, wrapped, sizeof(*wrapped) + wrapped->capacity * wrapped->element_size);
        *list = &wrapped[1];
    }
}

static void *i_get_index_ptr(i_n_list_wrapped_t *wrapped, uint32_t index) {
    return ((void*)&wrapped[1]) + index * wrapped->element_size;
}

extern void *n_list_create(n_allocator_t allocator, size_t elementSize, uint32_t capacity) {
    i_n_list_wrapped_t *wrapped = n_memory_allocator_alloc(allocator, sizeof(*wrapped) + capacity * elementSize);
    wrapped->allocator = allocator;
    wrapped->element_size = elementSize;
    wrapped->capacity = capacity;
    wrapped->length = 0;
    return &wrapped[1];
}

extern void *i_n_list_add_at(void **list, uint32_t index) {
    i_ensure_capacity(list, n_list_length(*list) + 1);
    i_n_list_wrapped_t *wrapped = i_wrap(*list);
    void *index_ptr = i_get_index_ptr(wrapped, index);
    memmove(index_ptr + wrapped->element_size, index_ptr, (wrapped->length - 1 - index) * wrapped->element_size);
    return index_ptr;
}

extern void *i_n_list_add(void **list) {
    i_ensure_capacity(list, n_list_length(*list) + 1);
    i_n_list_wrapped_t *wrapped = i_wrap(*list);
    return i_get_index_ptr(wrapped, wrapped->length - 1);
}

extern void n_list_remove_at(void *list, uint32_t index) {
    i_n_list_wrapped_t *wrapped = i_wrap(list);
    memcpy(i_get_index_ptr(wrapped, index), i_get_index_ptr(wrapped, wrapped->length - 1), wrapped->element_size);
    wrapped->length--;
}
extern void n_list_remove_at_ordered(void *list, uint32_t index) {
    i_n_list_wrapped_t *wrapped = i_wrap(list);
    void *index_ptr = i_get_index_ptr(wrapped, index);
    memmove(index_ptr, index_ptr + wrapped->element_size, (wrapped->length - 1 - index) * wrapped->element_size);
    wrapped->length--;
}

extern void n_list_clear(void *list) {
    i_n_list_wrapped_t *wrapped = i_wrap(list);
    wrapped->length = 0;
}

extern uint32_t n_list_length(void *list) {
    return i_wrap(list)->length;
}

extern uint32_t n_list_capacity(void *list) {
    return i_wrap(list)->capacity;
}

extern void i_n_list_trim_excess(void **list) {
    i_n_list_wrapped_t *wrapped = i_wrap(*list);
    if (wrapped->capacity > wrapped->length) {
        size_t size = sizeof(*wrapped) + wrapped->length * wrapped->element_size;
        i_n_list_wrapped_t *new_wrapped = n_memory_allocator_alloc(wrapped->allocator, size);
        memcpy(new_wrapped, wrapped, size);
        new_wrapped->capacity = wrapped->length;
        *list = (void*)&new_wrapped[1];
        n_memory_allocator_free(wrapped->allocator, wrapped);
    }
}

extern void n_list_destroy(void *list) {
    i_n_list_wrapped_t *wrapped = i_wrap(list);
    n_memory_allocator_free(wrapped->allocator, wrapped);
}

#else // TEST_BUILD

#ifdef TEST_IMPL

TEST("new list length equals 0") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_assert_u32_eq(0, n_list_length(list));
    n_list_destroy(list);
}

TEST("after add length increases by 1") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_assert_u32_eq(0, n_list_length(list));
    n_list_add(list, uint32_t, 12);
    n_assert_u32_eq(1, n_list_length(list));
    n_list_add(list, uint32_t, 10002341);
    n_assert_u32_eq(2, n_list_length(list));
    n_list_destroy(list);
}

TEST("after add values in the list are correct") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_list_add(list, uint32_t, 12);
    n_list_add(list, uint32_t, 10002341);
    n_assert_u32_eq(12, list[0]);
    n_assert_u32_eq(10002341, list[1]);
    n_list_destroy(list);
}

TEST("after more then capacity adds capacity doubles") {
    uint32_t capacity = 4;
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), capacity);
    for (uint32_t i = 0; i < capacity + 1; i++) {
        n_list_add(list, uint32_t, 7);
    }
    n_assert_u32_eq(capacity * 2, n_list_capacity(list));
    n_list_destroy(list);
}

TEST("add_at sets correct element values") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 1);
    n_list_add(list, uint32_t, 0);
    n_list_add(list, uint32_t, 1);
    n_list_add(list, uint32_t, 3);
    n_list_add(list, uint32_t, 4);
    n_list_add_at(list, uint32_t, 2, 2);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(1, list[1]);
    n_assert_u32_eq(2, list[2]);
    n_assert_u32_eq(3, list[3]);
    n_assert_u32_eq(4, list[4]);
    n_list_destroy(list);
}

TEST("remove_at sets correct element values") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 4);
    n_list_add(list, uint32_t, 0);
    n_list_add(list, uint32_t, 3);
    n_list_add(list, uint32_t, 1);
    n_list_add(list, uint32_t, 2);
    n_list_remove_at(list, 1);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(2, list[1]);
    n_assert_u32_eq(1, list[2]);
    n_list_destroy(list);
}

TEST("remove_at_ordered sets correct element values") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 4);
    n_list_add(list, uint32_t, 0);
    n_list_add(list, uint32_t, 3);
    n_list_add(list, uint32_t, 1);
    n_list_add(list, uint32_t, 2);
    n_list_remove_at_ordered(list, 1);
    n_assert_u32_eq(0, list[0]);
    n_assert_u32_eq(1, list[1]);
    n_assert_u32_eq(2, list[2]);
    n_list_destroy(list);
}

TEST("clear sets length to 0") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 4);
    n_list_add(list, uint32_t, 0);
    n_list_add(list, uint32_t, 1);
    n_list_clear(list);
    n_assert_u32_eq(0, n_list_length(list));
    n_list_destroy(list);
}

TEST("trim_excess sets capacity to length and keeps all elements the same") {
    NLIST(uint32_t) list = n_list_create(n_memory_get_default_allocator(), sizeof(*list), 8);
    n_list_add(list, uint32_t, 420);
    n_list_add(list, uint32_t, 981);
    n_list_add(list, uint32_t, 21);
    n_list_trim_excess(list);
    n_assert_u32_eq(420, list[0]);
    n_assert_u32_eq(981, list[1]);
    n_assert_u32_eq(21, list[2]);
    n_assert_u32_eq(3, n_list_capacity(list));
    n_list_destroy(list);
}

#endif // TEST_IMPL
#endif // TEST_BUILD
