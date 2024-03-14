#include "nandi.h"

#if PLATFORM == WINDOWS

#include <windows.h>

extern n_thread_t n_threading_thread_create(void (*func)(void *), void *args) {
    return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, args, 0, NULL);
}

extern void n_threading_thread_wait(n_thread_t thread) {
    WaitForSingleObject(thread, INFINITE);
}

extern void n_threading_thread_terminate(n_thread_t thread, int exitCode) {
    TerminateThread(thread, exitCode);
}

extern n_thread_t n_threading_get_current_thread() {
    return GetCurrentThread();
}

extern uint64_t n_threading_get_current_thread_id(n_thread_t thread) {
    return GetCurrentThreadId();
}

extern void n_threading_thread_sleep(uint64_t milliseconds) {
    Sleep(milliseconds);
}

extern n_mutex_t n_threading_mutex_create() {
    return CreateMutex(NULL, FALSE, "");
}

extern bool_t n_threading_mutex_wait(n_mutex_t mutex) {
    int64_t dwWaitResult = WaitForSingleObject(mutex, INFINITE);

    switch (dwWaitResult) {
        case WAIT_OBJECT_0:
            return 1;
            break;
        case WAIT_ABANDONED:
            return 0;
    }
    return 0;
}

extern bool_t n_threading_mutex_release(n_mutex_t mutex) {
    return ReleaseMutex(mutex);
}

#endif // PLATFORM == WINDOWS
