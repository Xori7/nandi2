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

extern void *n_memory_allocator_alloc(const n_allocator_t *allocator, size_t size);
extern void *n_memory_allocator_realloc(const n_allocator_t *allocator, void *ptr, size_t size);
extern void n_memory_allocator_free(const n_allocator_t *allocator, void *ptr);

const n_allocator_t *n_memory_get_default_allocator(void);

/*************************
Module: NList
Source: nlist.c
*************************/
#define LIST(type) type *

extern void *n_list_create(const n_allocator_t *allocator, size_t elementSize, uint32_t capacity);
void i_n_list_set_length(void *list, uint32_t length);
#define n_list_add(list, element) i_n_list_set_length(list, n_list_length(list) + 1); list[n_list_length(list) - 1] = element;
#define n_list_add_at(list, element, index) do {                    \
    i_n_list_set_length(list, n_list_length(list) + 1);             \
    for (uint32_t i = n_list_length(list) - 2; i >= index; i--) {   \
        list[i + 1] = list[i];                                      \
    }                                                               \
    list[index] = element;                                          \
}while(0);

#define n_list_remove_at(list, index) do {                          \
    list[index] = list[n_list_length(list) - 1];                    \
    i_n_list_set_length(list, n_list_length(list) - 1);             \
}while(0);

#define n_list_remove_at_ordered(list, index) do {                  \
    for (uint32_t i = index; i <= n_list_length(list) - 2; i++) {   \
        list[i] = list[i + 1];                                      \
    }                                                               \
    i_n_list_set_length(list, n_list_length(list) - 1);             \
}while(0);

extern uint32_t n_list_length(void *list);
extern uint32_t n_list_capacity(void *list);
extern void n_list_clear(void *list);
extern void n_list_sort(void *list);
extern void n_list_trim_excess(void *list);
extern void n_list_destroy(void *list);

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

/*************************
Module: NMath
Source: 
*************************/
typedef struct {
    uint32_t x, y;
} vec2u32_t;

/*************************
Module: NWindow
Source: nwindow_windows.c
*************************/
typedef struct i_n_window_t *n_window_t;
typedef void (*n_window_size_changed_tfn)(n_window_t window);

struct i_n_window_t {
    const n_allocator_t *allocator;
    const void *handle;
    const char *title;
    vec2u32_t size;
    n_window_size_changed_tfn on_size_changed_fn;
};

extern n_window_t n_window_create(const n_allocator_t *allocator, const char *title, n_window_size_changed_tfn onSizeChangedFn);
extern void n_window_destroy(n_window_t window);
extern void n_window_set_client_size(n_window_t window, vec2u32_t size);

/*************************
Module: NInput
Source: ninput_windows.c
*************************/
typedef enum {
    KC_LeftMouseBtn = 0x01, //Left mouse button
    KC_RightMouseBtn = 0x02, //Right mouse button
    KC_CtrlBrkPrcs = 0x03, //Control-break processing
    KC_MidMouseBtn = 0x04, //Middle mouse button

    KC_ThumbForward = 0x05, //Thumb button back on mouse aka X1
    KC_ThumbBack = 0x06, //Thumb button forward on mouse aka X2

    //0x07 : reserved

    KC_BackSpace = 0x08, //Backspace key
    KC_Tab = 0x09, //Tab key

    //0x0A - 0x0B : reserved

    KC_Clear = 0x0C, //Clear key
    KC_Enter = 0x0D, //Enter or Return key

    //0x0E - 0x0F : unassigned

    KC_Shift = 0x10, //Shift key
    KC_Control = 0x11, //Ctrl key
    KC_Alt = 0x12, //Alt key
    KC_Pause = 0x13, //Pause key
    KC_CapsLock = 0x14, //Caps lock key

    KC_Kana = 0x15, //Kana input mode
    KC_Hangeul = 0x15, //Hangeul input mode
    KC_Hangul = 0x15, //Hangul input mode

    //0x16 : unassigned

    KC_Junju = 0x17, //Junja input method
    KC_Final = 0x18, //Final input method
    KC_Hanja = 0x19, //Hanja input method
    KC_Kanji = 0x19, //Kanji input method

    //0x1A : unassigned

    KC_Escape = 0x1B, //Esc key

    KC_Convert = 0x1C, //IME convert
    KC_NonConvert = 0x1D, //IME Non convert
    KC_Accept = 0x1E, //IME accept
    KC_ModeChange = 0x1F, //IME mode change

    KC_Space = 0x20, //Space bar
    KC_PageUp = 0x21, //Page up key
    KC_PageDown = 0x22, //Page down key
    KC_End = 0x23, //End key
    KC_Home = 0x24, //Home key
    KC_LeftArrow = 0x25, //Left arrow key
    KC_UpArrow = 0x26, //Up arrow key
    KC_RightArrow = 0x27, //Right arrow key
    KC_DownArrow = 0x28, //Down arrow key
    KC_Select = 0x29, //Select key
    KC_Print = 0x2A, //Print key
    KC_Execute = 0x2B, //Execute key
    KC_PrintScreen = 0x2C, //Print screen key
    KC_Inser = 0x2D, //Insert key
    KC_Delete = 0x2E, //Delete key
    KC_Help = 0x2F, //Help key

    KC_Num0 = 0x30, //Top row 0 key (Matches '0')
    KC_Num1 = 0x31, //Top row 1 key (Matches '1')
    KC_Num2 = 0x32, //Top row 2 key (Matches '2')
    KC_Num3 = 0x33, //Top row 3 key (Matches '3')
    KC_Num4 = 0x34, //Top row 4 key (Matches '4')
    KC_Num5 = 0x35, //Top row 5 key (Matches '5')
    KC_Num6 = 0x36, //Top row 6 key (Matches '6')
    KC_Num7 = 0x37, //Top row 7 key (Matches '7')
    KC_Num8 = 0x38, //Top row 8 key (Matches '8')
    KC_Num9 = 0x39, //Top row 9 key (Matches '9')

    //0x3A - 0x40 : unassigned

    KC_A = 0x41, //A key (Matches 'A')
    KC_B = 0x42, //B key (Matches 'B')
    KC_C = 0x43, //C key (Matches 'C')
    KC_D = 0x44, //D key (Matches 'D')
    KC_E = 0x45, //E key (Matches 'E')
    KC_F = 0x46, //F key (Matches 'F')
    KC_G = 0x47, //G key (Matches 'G')
    KC_H = 0x48, //H key (Matches 'H')
    KC_I = 0x49, //I key (Matches 'I')
    KC_J = 0x4A, //J key (Matches 'J')
    KC_K = 0x4B, //K key (Matches 'K')
    KC_L = 0x4C, //L key (Matches 'L')
    KC_M = 0x4D, //M key (Matches 'M')
    KC_N = 0x4E, //N key (Matches 'N')
    KC_O = 0x4F, //O key (Matches 'O')
    KC_P = 0x50, //P key (Matches 'P')
    KC_Q = 0x51, //Q key (Matches 'Q')
    KC_R = 0x52, //R key (Matches 'R')
    KC_S = 0x53, //S key (Matches 'S')
    KC_T = 0x54, //T key (Matches 'T')
    KC_U = 0x55, //U key (Matches 'U')
    KC_V = 0x56, //V key (Matches 'V')
    KC_W = 0x57, //W key (Matches 'W')
    KC_X = 0x58, //X key (Matches 'X')
    KC_Y = 0x59, //Y key (Matches 'Y')
    KC_Z = 0x5A, //Z key (Matches 'Z')

    KC_LeftWin = 0x5B, //Left windows key
    KC_RightWin = 0x5C, //Right windows key
    KC_Apps = 0x5D, //Applications key

    //0x5E : reserved

    KC_Sleep = 0x5F, //Computer sleep key

    KC_Numpad0 = 0x60, //Numpad 0
    KC_Numpad1 = 0x61, //Numpad 1
    KC_Numpad2 = 0x62, //Numpad 2
    KC_Numpad3 = 0x63, //Numpad 3
    KC_Numpad4 = 0x64, //Numpad 4
    KC_Numpad5 = 0x65, //Numpad 5
    KC_Numpad6 = 0x66, //Numpad 6
    KC_Numpad7 = 0x67, //Numpad 7
    KC_Numpad8 = 0x68, //Numpad 8
    KC_Numpad9 = 0x69, //Numpad 9
    KC_Multiply = 0x6A, //Multiply key
    KC_Add = 0x6B, //Add key
    KC_Separator = 0x6C, //Separator key
    KC_Subtract = 0x6D, //Subtract key
    KC_Decimal = 0x6E, //Decimal key
    KC_Divide = 0x6F, //Divide key
    KC_F1 = 0x70, //F1
    KC_F2 = 0x71, //F2
    KC_F3 = 0x72, //F3
    KC_F4 = 0x73, //F4
    KC_F5 = 0x74, //F5
    KC_F6 = 0x75, //F6
    KC_F7 = 0x76, //F7
    KC_F8 = 0x77, //F8
    KC_F9 = 0x78, //F9
    KC_F10 = 0x79, //F10
    KC_F11 = 0x7A, //F11
    KC_F12 = 0x7B, //F12
    KC_F13 = 0x7C, //F13
    KC_F14 = 0x7D, //F14
    KC_F15 = 0x7E, //F15
    KC_F16 = 0x7F, //F16
    KC_F17 = 0x80, //F17
    KC_F18 = 0x81, //F18
    KC_F19 = 0x82, //F19
    KC_F20 = 0x83, //F20
    KC_F21 = 0x84, //F21
    KC_F22 = 0x85, //F22
    KC_F23 = 0x86, //F23
    KC_F24 = 0x87, //F24

    //0x88 - 0x8F : UI navigation

    KC_NavigationView = 0x88, //reserved
    KC_NavigationMenu = 0x89, //reserved
    KC_NavigationUp = 0x8A, //reserved
    KC_NavigationDown = 0x8B, //reserved
    KC_NavigationLeft = 0x8C, //reserved
    KC_NavigationRight = 0x8D, //reserved
    KC_NavigationAccept = 0x8E, //reserved
    KC_NavigationCancel = 0x8F, //reserved

    KC_NumLock = 0x90, //Num lock key
    KC_ScrollLock = 0x91, //Scroll lock key

    KC_NumpadEqual = 0x92, //Numpad =

    KC_FJ_Jisho = 0x92, //Dictionary key
    KC_FJ_Masshou = 0x93, //Unregister word key
    KC_FJ_Touroku = 0x94, //Register word key
    KC_FJ_Loya = 0x95, //Left OYAYUBI key
    KC_FJ_Roya = 0x96, //Right OYAYUBI key

    //0x97 - 0x9F : unassigned

    KC_LeftShift = 0xA0, //Left shift key
    KC_RightShift = 0xA1, //Right shift key
    KC_LeftCtrl = 0xA2, //Left control key
    KC_RightCtrl = 0xA3, //Right control key
    KC_LeftMenu = 0xA4, //Left menu key
    KC_RightMenu = 0xA5, //Right menu

    KC_BrowserBack = 0xA6, //Browser back button
    KC_BrowserForward = 0xA7, //Browser forward button
    KC_BrowserRefresh = 0xA8, //Browser refresh button
    KC_BrowserStop = 0xA9, //Browser stop button
    KC_BrowserSearch = 0xAA, //Browser search button
    KC_BrowserFavorites = 0xAB, //Browser favorites button
    KC_BrowserHome = 0xAC, //Browser home button

    KC_VolumeMute = 0xAD, //Volume mute button
    KC_VolumeDown = 0xAE, //Volume down button
    KC_VolumeUp = 0xAF, //Volume up button
    KC_NextTrack = 0xB0, //Next track media button
    KC_PrevTrack = 0xB1, //Previous track media button
    KC_Stop = 0xB2, //Stop media button
    KC_PlayPause = 0xB3, //Play/pause media button
    KC_Mail = 0xB4, //Launch mail button
    KC_MediaSelect = 0xB5, //Launch media select button
    KC_App1 = 0xB6, //Launch app 1 button
    KC_App2 = 0xB7, //Launch app 2 button

    //0xB8 - 0xB9 : reserved

    KC_OEM1 = 0xBA, //;: key for US or misc keys for others
    KC_Plus = 0xBB, //Plus key
    KC_Comma = 0xBC, //Comma key
    KC_Minus = 0xBD, //Minus key
    KC_Period = 0xBE, //Period key
    KC_OEM2 = 0xBF, //? for US or misc keys for others
    KC_OEM3 = 0xC0, //~ for US or misc keys for others

    //0xC1 - 0xC2 : reserved

    KC_Gamepad_A = 0xC3, //Gamepad A button
    KC_Gamepad_B = 0xC4, //Gamepad B button
    KC_Gamepad_X = 0xC5, //Gamepad X button
    KC_Gamepad_Y = 0xC6, //Gamepad Y button
    KC_GamepadRightBumper = 0xC7, //Gamepad right bumper
    KC_GamepadLeftBumper = 0xC8, //Gamepad left bumper
    KC_GamepadLeftTrigger = 0xC9, //Gamepad left trigger
    KC_GamepadRightTrigger = 0xCA, //Gamepad right trigger
    KC_GamepadDPadUp = 0xCB, //Gamepad DPad up
    KC_GamepadDPadDown = 0xCC, //Gamepad DPad down
    KC_GamepadDPadLeft = 0xCD, //Gamepad DPad left
    KC_GamepadDPadRight = 0xCE, //Gamepad DPad right
    KC_GamepadMenu = 0xCF, //Gamepad menu button
    KC_GamepadView = 0xD0, //Gamepad view button
    KC_GamepadLeftStickBtn = 0xD1, //Gamepad left stick button
    KC_GamepadRightStickBtn = 0xD2, //Gamepad right stick button
    KC_GamepadLeftStickUp = 0xD3, //Gamepad left stick up
    KC_GamepadLeftStickDown = 0xD4, //Gamepad left stick down
    KC_GamepadLeftStickRight = 0xD5, //Gamepad left stick right
    KC_GamepadLeftStickLeft = 0xD6, //Gamepad left stick left
    KC_GamepadRightStickUp = 0xD7, //Gamepad right stick up
    KC_GamepadRightStickDown = 0xD8, //Gamepad right stick down
    KC_GamepadRightStickRight = 0xD9, //Gamepad right stick right
    KC_GamepadRightStickLeft = 0xDA, //Gamepad right stick left

    KC_OEM4 = 0xDB, //[ for US or misc keys for others
    KC_OEM5 = 0xDC, //\ for US or misc keys for others
    KC_OEM6 = 0xDD, //] for US or misc keys for others
    KC_OEM7 = 0xDE, //' for US or misc keys for others
    KC_OEM8 = 0xDF, //Misc keys for others

    //0xE0 : reserved

    KC_OEMAX = 0xE1, //AX key on Japanese AX keyboard
    KC_OEM102 = 0xE2, //"<>" or "\|" on RT 102-key keyboard
    KC_ICOHelp = 0xE3, //Help key on ICO
    KC_ICO00 = 0xE4, //00 key on ICO

    KC_ProcessKey = 0xE5, //Process key input method
    KC_OEMCLEAR = 0xE6, //OEM specific
    KC_Packet = 0xE7, //IDK man try to google it
} n_key_code_enum;

extern void n_input_update();
extern bool_t n_input_key(n_key_code_enum keyCode);
extern bool_t n_input_key_down(n_key_code_enum keyCode);
extern bool_t n_input_key_up(n_key_code_enum keyCode);
extern vec2u32_t n_input_cursor_position();
extern int32_t n_input_mouse_wheel();

/*************************
Module: 
Source: .c
*************************/


// vi: ft=c

