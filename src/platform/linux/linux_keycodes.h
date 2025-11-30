#pragma once

#ifdef CROSSINPUT_LINUX

#include "../../../include/CrossInput.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>

// X11 defines KeyPress and KeyRelease as macros, which conflicts with our function names
// Save and undefine them
#ifdef KeyPress
#define X11_KeyPress KeyPress
#define X11_KeyRelease KeyRelease
#undef KeyPress
#undef KeyRelease
#endif

// Define evdev keycodes ourselves to avoid conflicts with our KeyCode enum
// These values are from <linux/input-event-codes.h>
namespace EvdevCodes
{
    // Alphanumeric keys
    constexpr unsigned int EVDEV_KEY_A = 30;
    constexpr unsigned int EVDEV_KEY_B = 48;
    constexpr unsigned int EVDEV_KEY_C = 46;
    constexpr unsigned int EVDEV_KEY_D = 32;
    constexpr unsigned int EVDEV_KEY_E = 18;
    constexpr unsigned int EVDEV_KEY_F = 33;
    constexpr unsigned int EVDEV_KEY_G = 34;
    constexpr unsigned int EVDEV_KEY_H = 35;
    constexpr unsigned int EVDEV_KEY_I = 23;
    constexpr unsigned int EVDEV_KEY_J = 36;
    constexpr unsigned int EVDEV_KEY_K = 37;
    constexpr unsigned int EVDEV_KEY_L = 38;
    constexpr unsigned int EVDEV_KEY_M = 50;
    constexpr unsigned int EVDEV_KEY_N = 49;
    constexpr unsigned int EVDEV_KEY_O = 24;
    constexpr unsigned int EVDEV_KEY_P = 25;
    constexpr unsigned int EVDEV_KEY_Q = 16;
    constexpr unsigned int EVDEV_KEY_R = 19;
    constexpr unsigned int EVDEV_KEY_S = 31;
    constexpr unsigned int EVDEV_KEY_T = 20;
    constexpr unsigned int EVDEV_KEY_U = 22;
    constexpr unsigned int EVDEV_KEY_V = 47;
    constexpr unsigned int EVDEV_KEY_W = 17;
    constexpr unsigned int EVDEV_KEY_X = 45;
    constexpr unsigned int EVDEV_KEY_Y = 21;
    constexpr unsigned int EVDEV_KEY_Z = 44;

    // Number keys
    constexpr unsigned int EVDEV_KEY_0 = 11;
    constexpr unsigned int EVDEV_KEY_1 = 2;
    constexpr unsigned int EVDEV_KEY_2 = 3;
    constexpr unsigned int EVDEV_KEY_3 = 4;
    constexpr unsigned int EVDEV_KEY_4 = 5;
    constexpr unsigned int EVDEV_KEY_5 = 6;
    constexpr unsigned int EVDEV_KEY_6 = 7;
    constexpr unsigned int EVDEV_KEY_7 = 8;
    constexpr unsigned int EVDEV_KEY_8 = 9;
    constexpr unsigned int EVDEV_KEY_9 = 10;

    // Function keys
    constexpr unsigned int EVDEV_KEY_F1 = 59;
    constexpr unsigned int EVDEV_KEY_F2 = 60;
    constexpr unsigned int EVDEV_KEY_F3 = 61;
    constexpr unsigned int EVDEV_KEY_F4 = 62;
    constexpr unsigned int EVDEV_KEY_F5 = 63;
    constexpr unsigned int EVDEV_KEY_F6 = 64;
    constexpr unsigned int EVDEV_KEY_F7 = 65;
    constexpr unsigned int EVDEV_KEY_F8 = 66;
    constexpr unsigned int EVDEV_KEY_F9 = 67;
    constexpr unsigned int EVDEV_KEY_F10 = 68;
    constexpr unsigned int EVDEV_KEY_F11 = 87;
    constexpr unsigned int EVDEV_KEY_F12 = 88;

    // Control keys
    constexpr unsigned int EVDEV_KEY_ESC = 1;
    constexpr unsigned int EVDEV_KEY_SPACE = 57;
    constexpr unsigned int EVDEV_KEY_ENTER = 28;
    constexpr unsigned int EVDEV_KEY_TAB = 15;
    constexpr unsigned int EVDEV_KEY_LEFTSHIFT = 42;
    constexpr unsigned int EVDEV_KEY_LEFTCTRL = 29;
    constexpr unsigned int EVDEV_KEY_LEFTALT = 56;
    constexpr unsigned int EVDEV_KEY_CAPSLOCK = 58;
    constexpr unsigned int EVDEV_KEY_BACKSPACE = 14;
    constexpr unsigned int EVDEV_KEY_DELETE = 111;
    constexpr unsigned int EVDEV_KEY_INSERT = 110;

    // Arrow keys
    constexpr unsigned int EVDEV_KEY_LEFT = 105;
    constexpr unsigned int EVDEV_KEY_RIGHT = 106;
    constexpr unsigned int EVDEV_KEY_UP = 103;
    constexpr unsigned int EVDEV_KEY_DOWN = 108;

    // Symbol keys
    constexpr unsigned int EVDEV_KEY_COMMA = 51;
    constexpr unsigned int EVDEV_KEY_DOT = 52;
    constexpr unsigned int EVDEV_KEY_SEMICOLON = 39;
    constexpr unsigned int EVDEV_KEY_APOSTROPHE = 40;
    constexpr unsigned int EVDEV_KEY_SLASH = 53;
    constexpr unsigned int EVDEV_KEY_BACKSLASH = 43;

    // Mouse buttons
    constexpr unsigned int EVDEV_BTN_LEFT = 0x110;
    constexpr unsigned int EVDEV_BTN_RIGHT = 0x111;
    constexpr unsigned int EVDEV_BTN_MIDDLE = 0x112;
}

namespace CrossInput
{
    namespace Internal
    {

        // Linux (X11): Maps CrossInput::KeyCode to X11 KeySym
        inline unsigned long keycode_to_x11_keysym(KeyCode key)
        {
            switch (key)
            {
            // Alphanumeric Keys (a-z lowercase KeySyms)
            case KeyCode::KEY_A:
                return XK_a;
            case KeyCode::KEY_B:
                return XK_b;
            case KeyCode::KEY_C:
                return XK_c;
            case KeyCode::KEY_D:
                return XK_d;
            case KeyCode::KEY_E:
                return XK_e;
            case KeyCode::KEY_F:
                return XK_f;
            case KeyCode::KEY_G:
                return XK_g;
            case KeyCode::KEY_H:
                return XK_h;
            case KeyCode::KEY_I:
                return XK_i;
            case KeyCode::KEY_J:
                return XK_j;
            case KeyCode::KEY_K:
                return XK_k;
            case KeyCode::KEY_L:
                return XK_l;
            case KeyCode::KEY_M:
                return XK_m;
            case KeyCode::KEY_N:
                return XK_n;
            case KeyCode::KEY_O:
                return XK_o;
            case KeyCode::KEY_P:
                return XK_p;
            case KeyCode::KEY_Q:
                return XK_q;
            case KeyCode::KEY_R:
                return XK_r;
            case KeyCode::KEY_S:
                return XK_s;
            case KeyCode::KEY_T:
                return XK_t;
            case KeyCode::KEY_U:
                return XK_u;
            case KeyCode::KEY_V:
                return XK_v;
            case KeyCode::KEY_W:
                return XK_w;
            case KeyCode::KEY_X:
                return XK_x;
            case KeyCode::KEY_Y:
                return XK_y;
            case KeyCode::KEY_Z:
                return XK_z;

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return XK_0;
            case KeyCode::KEY_1:
                return XK_1;
            case KeyCode::KEY_2:
                return XK_2;
            case KeyCode::KEY_3:
                return XK_3;
            case KeyCode::KEY_4:
                return XK_4;
            case KeyCode::KEY_5:
                return XK_5;
            case KeyCode::KEY_6:
                return XK_6;
            case KeyCode::KEY_7:
                return XK_7;
            case KeyCode::KEY_8:
                return XK_8;
            case KeyCode::KEY_9:
                return XK_9;

            // Function Keys (F1-F12)
            case KeyCode::KEY_F1:
                return XK_F1;
            case KeyCode::KEY_F2:
                return XK_F2;
            case KeyCode::KEY_F3:
                return XK_F3;
            case KeyCode::KEY_F4:
                return XK_F4;
            case KeyCode::KEY_F5:
                return XK_F5;
            case KeyCode::KEY_F6:
                return XK_F6;
            case KeyCode::KEY_F7:
                return XK_F7;
            case KeyCode::KEY_F8:
                return XK_F8;
            case KeyCode::KEY_F9:
                return XK_F9;
            case KeyCode::KEY_F10:
                return XK_F10;
            case KeyCode::KEY_F11:
                return XK_F11;
            case KeyCode::KEY_F12:
                return XK_F12;

            // Control Keys
            case KeyCode::KEY_ESCAPE:
                return XK_Escape;
            case KeyCode::KEY_SPACE:
                return XK_space;
            case KeyCode::KEY_ENTER:
                return XK_Return;
            case KeyCode::KEY_TAB:
                return XK_Tab;
            case KeyCode::KEY_SHIFT:
                return XK_Shift_L;
            case KeyCode::KEY_CONTROL:
                return XK_Control_L;
            case KeyCode::KEY_ALT:
                return XK_Alt_L;
            case KeyCode::KEY_CAPS_LOCK:
                return XK_Caps_Lock;
            case KeyCode::KEY_BACKSPACE:
                return XK_BackSpace;
            case KeyCode::KEY_DELETE:
                return XK_Delete;
            case KeyCode::KEY_INSERT:
                return XK_Insert;

            // Arrow Keys
            case KeyCode::KEY_LEFT:
                return XK_Left;
            case KeyCode::KEY_RIGHT:
                return XK_Right;
            case KeyCode::KEY_UP:
                return XK_Up;
            case KeyCode::KEY_DOWN:
                return XK_Down;

            // Symbol Keys
            case KeyCode::KEY_COMMA:
                return XK_comma;
            case KeyCode::KEY_PERIOD:
                return XK_period;
            case KeyCode::KEY_SEMICOLON:
                return XK_semicolon;
            case KeyCode::KEY_APOSTROPHE:
                return XK_apostrophe;
            case KeyCode::KEY_SLASH:
                return XK_slash;
            case KeyCode::KEY_BACKSLASH:
                return XK_backslash;

            default:
                return 0;
            }
        }

        // Mouse button to X11 button code mapping
        inline unsigned int mouse_button_to_x11_button(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return 1; // Button1
            case MouseButton::Right:
                return 3; // Button3
            case MouseButton::Middle:
                return 2; // Button2
            default:
                return 0;
            }
        }

        // Linux evdev keycode mapping for Wayland/libei
        // Maps CrossInput::KeyCode to Linux evdev keycodes (KEY_*)
        inline unsigned int keycode_to_evdev(KeyCode key)
        {
            switch (key)
            {
            // Alphanumeric Keys (A-Z)
            case KeyCode::KEY_A:
                return EvdevCodes::EVDEV_KEY_A;
            case KeyCode::KEY_B:
                return EvdevCodes::EVDEV_KEY_B;
            case KeyCode::KEY_C:
                return EvdevCodes::EVDEV_KEY_C;
            case KeyCode::KEY_D:
                return EvdevCodes::EVDEV_KEY_D;
            case KeyCode::KEY_E:
                return EvdevCodes::EVDEV_KEY_E;
            case KeyCode::KEY_F:
                return EvdevCodes::EVDEV_KEY_F;
            case KeyCode::KEY_G:
                return EvdevCodes::EVDEV_KEY_G;
            case KeyCode::KEY_H:
                return EvdevCodes::EVDEV_KEY_H;
            case KeyCode::KEY_I:
                return EvdevCodes::EVDEV_KEY_I;
            case KeyCode::KEY_J:
                return EvdevCodes::EVDEV_KEY_J;
            case KeyCode::KEY_K:
                return EvdevCodes::EVDEV_KEY_K;
            case KeyCode::KEY_L:
                return EvdevCodes::EVDEV_KEY_L;
            case KeyCode::KEY_M:
                return EvdevCodes::EVDEV_KEY_M;
            case KeyCode::KEY_N:
                return EvdevCodes::EVDEV_KEY_N;
            case KeyCode::KEY_O:
                return EvdevCodes::EVDEV_KEY_O;
            case KeyCode::KEY_P:
                return EvdevCodes::EVDEV_KEY_P;
            case KeyCode::KEY_Q:
                return EvdevCodes::EVDEV_KEY_Q;
            case KeyCode::KEY_R:
                return EvdevCodes::EVDEV_KEY_R;
            case KeyCode::KEY_S:
                return EvdevCodes::EVDEV_KEY_S;
            case KeyCode::KEY_T:
                return EvdevCodes::EVDEV_KEY_T;
            case KeyCode::KEY_U:
                return EvdevCodes::EVDEV_KEY_U;
            case KeyCode::KEY_V:
                return EvdevCodes::EVDEV_KEY_V;
            case KeyCode::KEY_W:
                return EvdevCodes::EVDEV_KEY_W;
            case KeyCode::KEY_X:
                return EvdevCodes::EVDEV_KEY_X;
            case KeyCode::KEY_Y:
                return EvdevCodes::EVDEV_KEY_Y;
            case KeyCode::KEY_Z:
                return EvdevCodes::EVDEV_KEY_Z;

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return EvdevCodes::EVDEV_KEY_0;
            case KeyCode::KEY_1:
                return EvdevCodes::EVDEV_KEY_1;
            case KeyCode::KEY_2:
                return EvdevCodes::EVDEV_KEY_2;
            case KeyCode::KEY_3:
                return EvdevCodes::EVDEV_KEY_3;
            case KeyCode::KEY_4:
                return EvdevCodes::EVDEV_KEY_4;
            case KeyCode::KEY_5:
                return EvdevCodes::EVDEV_KEY_5;
            case KeyCode::KEY_6:
                return EvdevCodes::EVDEV_KEY_6;
            case KeyCode::KEY_7:
                return EvdevCodes::EVDEV_KEY_7;
            case KeyCode::KEY_8:
                return EvdevCodes::EVDEV_KEY_8;
            case KeyCode::KEY_9:
                return EvdevCodes::EVDEV_KEY_9;

            // Function Keys (F1-F12)
            case KeyCode::KEY_F1:
                return EvdevCodes::EVDEV_KEY_F1;
            case KeyCode::KEY_F2:
                return EvdevCodes::EVDEV_KEY_F2;
            case KeyCode::KEY_F3:
                return EvdevCodes::EVDEV_KEY_F3;
            case KeyCode::KEY_F4:
                return EvdevCodes::EVDEV_KEY_F4;
            case KeyCode::KEY_F5:
                return EvdevCodes::EVDEV_KEY_F5;
            case KeyCode::KEY_F6:
                return EvdevCodes::EVDEV_KEY_F6;
            case KeyCode::KEY_F7:
                return EvdevCodes::EVDEV_KEY_F7;
            case KeyCode::KEY_F8:
                return EvdevCodes::EVDEV_KEY_F8;
            case KeyCode::KEY_F9:
                return EvdevCodes::EVDEV_KEY_F9;
            case KeyCode::KEY_F10:
                return EvdevCodes::EVDEV_KEY_F10;
            case KeyCode::KEY_F11:
                return EvdevCodes::EVDEV_KEY_F11;
            case KeyCode::KEY_F12:
                return EvdevCodes::EVDEV_KEY_F12;

            // Control Keys
            case KeyCode::KEY_ESCAPE:
                return EvdevCodes::EVDEV_KEY_ESC;
            case KeyCode::KEY_SPACE:
                return EvdevCodes::EVDEV_KEY_SPACE;
            case KeyCode::KEY_ENTER:
                return EvdevCodes::EVDEV_KEY_ENTER;
            case KeyCode::KEY_TAB:
                return EvdevCodes::EVDEV_KEY_TAB;
            case KeyCode::KEY_SHIFT:
                return EvdevCodes::EVDEV_KEY_LEFTSHIFT;
            case KeyCode::KEY_CONTROL:
                return EvdevCodes::EVDEV_KEY_LEFTCTRL;
            case KeyCode::KEY_ALT:
                return EvdevCodes::EVDEV_KEY_LEFTALT;
            case KeyCode::KEY_CAPS_LOCK:
                return EvdevCodes::EVDEV_KEY_CAPSLOCK;
            case KeyCode::KEY_BACKSPACE:
                return EvdevCodes::EVDEV_KEY_BACKSPACE;
            case KeyCode::KEY_DELETE:
                return EvdevCodes::EVDEV_KEY_DELETE;
            case KeyCode::KEY_INSERT:
                return EvdevCodes::EVDEV_KEY_INSERT;

            // Arrow Keys
            case KeyCode::KEY_LEFT:
                return EvdevCodes::EVDEV_KEY_LEFT;
            case KeyCode::KEY_RIGHT:
                return EvdevCodes::EVDEV_KEY_RIGHT;
            case KeyCode::KEY_UP:
                return EvdevCodes::EVDEV_KEY_UP;
            case KeyCode::KEY_DOWN:
                return EvdevCodes::EVDEV_KEY_DOWN;

            // Symbol Keys
            case KeyCode::KEY_COMMA:
                return EvdevCodes::EVDEV_KEY_COMMA;
            case KeyCode::KEY_PERIOD:
                return EvdevCodes::EVDEV_KEY_DOT;
            case KeyCode::KEY_SEMICOLON:
                return EvdevCodes::EVDEV_KEY_SEMICOLON;
            case KeyCode::KEY_APOSTROPHE:
                return EvdevCodes::EVDEV_KEY_APOSTROPHE;
            case KeyCode::KEY_SLASH:
                return EvdevCodes::EVDEV_KEY_SLASH;
            case KeyCode::KEY_BACKSLASH:
                return EvdevCodes::EVDEV_KEY_BACKSLASH;

            default:
                return 0;
            }
        }

        // Mouse button to evdev button code mapping for Wayland
        inline unsigned int mouse_button_to_evdev(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return EvdevCodes::EVDEV_BTN_LEFT;
            case MouseButton::Right:
                return EvdevCodes::EVDEV_BTN_RIGHT;
            case MouseButton::Middle:
                return EvdevCodes::EVDEV_BTN_MIDDLE;
            default:
                return 0;
            }
        }

    } // namespace Internal
} // namespace CrossInput

#endif // CROSSINPUT_LINUX
