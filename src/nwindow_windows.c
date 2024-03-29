#ifndef TEST_BUILD
#if PLATFORM == WINDOWS
#include "nandi.h"
#include <windows.h>
#include <commctrl.h>

void update_client_rect(n_window_t window) {
    RECT rect;
    GetClientRect((HWND)window->handle, &rect);
    window->size.x = rect.right - rect.left;
    window->size.y = rect.bottom - rect.top;
    if (window->on_size_changed_fn != NULL) {
        (window->on_size_changed_fn)(window);
    }
}

LRESULT WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (message) {
        case WM_SIZE:
            update_client_rect((n_window_t)dwRefData);
            break;
        case WM_QUIT:
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(window, message, wparam, lparam);
}

extern n_window_t n_window_create(const n_allocator_t *allocator, const char *title, n_window_size_changed_tfn onSizeChangedFn) {
    WNDCLASS windowClass = {
            .lpszClassName = title,
            .hInstance = GetModuleHandle(NULL),
            .lpfnWndProc = DefWindowProc,
    };
    RegisterClass(&windowClass);
    HWND windowHandle = CreateWindow(title, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, windowClass.hInstance, 0);
    n_window_t window = n_memory_allocator_alloc(allocator, sizeof *window);
    window->allocator = allocator;
    window->handle = windowHandle;
    window->title = title;
    window->on_size_changed_fn = onSizeChangedFn;
    update_client_rect(window);

    SetWindowSubclass((HWND)window->handle, WindowProc, 0, (DWORD_PTR)window);
    ShowWindow((HWND)window->handle, SHOW_OPENWINDOW);
    return window;
}

extern void n_window_destroy(n_window_t window) {
    DestroyWindow((HWND)window->handle);
    n_memory_allocator_free(window->allocator, window);
}

extern void n_window_set_client_size(n_window_t window, vec2u32_t size) {
    RECT rect = {
            .left = 0,
            .top = 0,
            .right = size.x,
            .bottom = size.y
    };
    if (!AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 0))
        RaiseException(GetLastError(), 0, 0, 0);
    SetWindowPos((HWND)window->handle, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
    UpdateWindow((HWND)window->handle);
}

#endif // PLATFORM == WINDOWS
#endif // TEST_BUILD

#ifdef TEST_INCLUDE
#include <stdlib.h>
#include <stdio.h>

#endif // TEST_INCLUDE

#ifdef TEST_IMPL
{
    n_window_t window = n_window_create(n_memory_get_default_allocator(), "Helloo", NULL);
    n_window_destroy(window);
    n_assert_i32_eq(10, 5);
    n_assert_i32_eq(10, 10);
    n_assert_i32_greater(36, 10);
}
#endif // TEST_IMPL
