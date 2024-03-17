#ifndef TEST_BUILD
#if PLATFORM == WINDOWS
#include "nandi.h"
#include <windows.h>

typedef enum {
    N_KEY_DOWN,
    N_KEY_UP,
    N_CURSOR_MOVE,
    N_MOUSE_BUTTON_DOWN,
    N_MOUSE_BUTTON_UP,
    N_MOUSE_WHEEL
} n_input_message_type_enum;

typedef struct {
    bool_t is_pressed;
    bool_t changed;
} i_n_key_state_t;

const uint32_t KEY_COUNT = 256;
typedef struct {
    vec2u32_t cursor_position;
    int32_t mouse_wheel;
    i_n_key_state_t keys[256];
} input_data_t;

input_data_t inputData = {0};

void process_input_message(n_input_message_type_enum type, uint32_t data) {
    switch (type) {
        case N_KEY_DOWN:
        case N_MOUSE_BUTTON_DOWN:
            data &= 0xff;
            inputData.keys[data].changed = 1;
            inputData.keys[data].is_pressed = 1;
            break;
        case N_KEY_UP:
        case N_MOUSE_BUTTON_UP:
            data &= 0xff;
            inputData.keys[data].changed = 1;
            inputData.keys[data].is_pressed = 0;
            break;
        case N_CURSOR_MOVE:
            inputData.cursor_position.x = data & 0xffff;
            inputData.cursor_position.y = data >> 16;
            break;
        case N_MOUSE_WHEEL:
            inputData.mouse_wheel = (int32_t)data;
            break;
    }
}

extern void n_input_update() {
    MSG msg = {0};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        switch(msg.message){
            case WM_KEYDOWN: {
                process_input_message(N_KEY_DOWN, msg.wParam);
                break;
            }
            case WM_KEYUP: {
                process_input_message(N_KEY_UP, msg.wParam);
                break;
            }
            case WM_MOUSEMOVE: {
                process_input_message(N_CURSOR_MOVE, msg.lParam);
                break;
            }
            case WM_LBUTTONDOWN: {
                process_input_message(N_MOUSE_BUTTON_DOWN, 1);
                break;
            }
            case WM_RBUTTONDOWN: {
                process_input_message(N_MOUSE_BUTTON_DOWN, 2);
                break;
            }
            case WM_LBUTTONUP: {
                process_input_message(N_MOUSE_BUTTON_UP, 1);
                break;
            }
            case WM_RBUTTONUP: {
                process_input_message(N_MOUSE_BUTTON_UP, 2);
                break;
            }
            case WM_MOUSEWHEEL: {
                uint32_t data = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                process_input_message(N_MOUSE_WHEEL, data);
                break;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

extern bool_t n_input_key(n_key_code_enum keyCode) {
    return inputData.keys[keyCode].is_pressed;
}

extern bool_t n_input_key_down(n_key_code_enum keyCode) {
    return inputData.keys[keyCode].is_pressed && inputData.keys[keyCode].changed;
}

extern bool_t n_input_key_up(n_key_code_enum keyCode) {
    return !inputData.keys[keyCode].is_pressed && inputData.keys[keyCode].changed;
}

extern vec2u32_t n_input_cursor_position() {
    return inputData.cursor_position;
}

extern int32_t n_input_mouse_wheel() {
    return inputData.mouse_wheel;
}

#endif // PLATFORM == WINDOWS
#endif // TEST_BUILD
