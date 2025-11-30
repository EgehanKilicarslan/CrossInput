#pragma once

#include "../include/CrossInput.h"

// Platform detection
#ifdef _WIN32
#define CROSSINPUT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__linux__)
#define CROSSINPUT_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <cstdlib>
#include <cstring>

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

// Check if libei is available at compile time
#ifdef CROSSINPUT_HAS_LIBEI
#include <libei.h>
#include <poll.h>
#include <unistd.h>
#include <gio/gio.h>
#include <gio/gunixfdlist.h>
#endif
#endif

namespace CrossInput
{
    namespace Internal
    {

// Wayland detection for Linux
#ifdef CROSSINPUT_LINUX
        inline bool IsWayland()
        {
            return std::getenv("WAYLAND_DISPLAY") != nullptr;
        }

        // Check if we're in a Wayland session (even if WAYLAND_DISPLAY is unset)
        inline bool IsWaylandSession()
        {
            const char *session_type = std::getenv("XDG_SESSION_TYPE");
            return session_type && std::string(session_type) == "wayland";
        }

        // Check if X11/XWayland is available for reading state
        inline bool HasX11Display()
        {
            return std::getenv("DISPLAY") != nullptr;
        }
#endif

        // =============================================================================
        // KEYCODE TO OS-SPECIFIC CODE MAPPING
        // =============================================================================

#ifdef CROSSINPUT_WINDOWS

        // Windows: Maps CrossInput::KeyCode to Windows Virtual Key Code (VK_CODE)
        inline int keycode_to_os_code(KeyCode key)
        {
            switch (key)
            {
            // Alphanumeric Keys (A-Z)
            case KeyCode::KEY_A:
                return 0x41;
            case KeyCode::KEY_B:
                return 0x42;
            case KeyCode::KEY_C:
                return 0x43;
            case KeyCode::KEY_D:
                return 0x44;
            case KeyCode::KEY_E:
                return 0x45;
            case KeyCode::KEY_F:
                return 0x46;
            case KeyCode::KEY_G:
                return 0x47;
            case KeyCode::KEY_H:
                return 0x48;
            case KeyCode::KEY_I:
                return 0x49;
            case KeyCode::KEY_J:
                return 0x4A;
            case KeyCode::KEY_K:
                return 0x4B;
            case KeyCode::KEY_L:
                return 0x4C;
            case KeyCode::KEY_M:
                return 0x4D;
            case KeyCode::KEY_N:
                return 0x4E;
            case KeyCode::KEY_O:
                return 0x4F;
            case KeyCode::KEY_P:
                return 0x50;
            case KeyCode::KEY_Q:
                return 0x51;
            case KeyCode::KEY_R:
                return 0x52;
            case KeyCode::KEY_S:
                return 0x53;
            case KeyCode::KEY_T:
                return 0x54;
            case KeyCode::KEY_U:
                return 0x55;
            case KeyCode::KEY_V:
                return 0x56;
            case KeyCode::KEY_W:
                return 0x57;
            case KeyCode::KEY_X:
                return 0x58; // VK_X
            case KeyCode::KEY_Y:
                return 0x59;
            case KeyCode::KEY_Z:
                return 0x5A; // VK_Z

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return 0x30; // VK_0
            case KeyCode::KEY_1:
                return 0x31;
            case KeyCode::KEY_2:
                return 0x32;
            case KeyCode::KEY_3:
                return 0x33;
            case KeyCode::KEY_4:
                return 0x34;
            case KeyCode::KEY_5:
                return 0x35;
            case KeyCode::KEY_6:
                return 0x36;
            case KeyCode::KEY_7:
                return 0x37;
            case KeyCode::KEY_8:
                return 0x38;
            case KeyCode::KEY_9:
                return 0x39;

            // Function Keys (F1-F12)
            case KeyCode::KEY_F1:
                return VK_F1;
            case KeyCode::KEY_F2:
                return VK_F2;
            case KeyCode::KEY_F3:
                return VK_F3;
            case KeyCode::KEY_F4:
                return VK_F4;
            case KeyCode::KEY_F5:
                return VK_F5;
            case KeyCode::KEY_F6:
                return VK_F6;
            case KeyCode::KEY_F7:
                return VK_F7;
            case KeyCode::KEY_F8:
                return VK_F8;
            case KeyCode::KEY_F9:
                return VK_F9;
            case KeyCode::KEY_F10:
                return VK_F10;
            case KeyCode::KEY_F11:
                return VK_F11;
            case KeyCode::KEY_F12:
                return VK_F12;

            // Control Keys
            case KeyCode::KEY_ESCAPE:
                return VK_ESCAPE;
            case KeyCode::KEY_SPACE:
                return VK_SPACE;
            case KeyCode::KEY_ENTER:
                return VK_RETURN;
            case KeyCode::KEY_TAB:
                return VK_TAB;
            case KeyCode::KEY_SHIFT:
                return VK_LSHIFT; // Left Shift
            case KeyCode::KEY_CONTROL:
                return VK_LCONTROL; // Left Control
            case KeyCode::KEY_ALT:
                return VK_LMENU; // Left Alt
            case KeyCode::KEY_CAPS_LOCK:
                return VK_CAPITAL;
            case KeyCode::KEY_BACKSPACE:
                return VK_BACK;
            case KeyCode::KEY_DELETE:
                return VK_DELETE;
            case KeyCode::KEY_INSERT:
                return VK_INSERT;

            // Arrow Keys
            case KeyCode::KEY_LEFT:
                return VK_LEFT;
            case KeyCode::KEY_RIGHT:
                return VK_RIGHT;
            case KeyCode::KEY_UP:
                return VK_UP;
            case KeyCode::KEY_DOWN:
                return VK_DOWN;

            // Symbol Keys
            case KeyCode::KEY_COMMA:
                return VK_OEM_COMMA; // ,
            case KeyCode::KEY_PERIOD:
                return VK_OEM_PERIOD; // .
            case KeyCode::KEY_SEMICOLON:
                return VK_OEM_1; // ;
            case KeyCode::KEY_APOSTROPHE:
                return VK_OEM_7; // '
            case KeyCode::KEY_SLASH:
                return VK_OEM_2; // /
            case KeyCode::KEY_BACKSLASH:
                return VK_OEM_5; // \ 

            default:
                return 0;
            }
        }

        // Mouse button to Windows flags mapping
        inline DWORD mouse_button_to_down_flag(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return MOUSEEVENTF_LEFTDOWN;
            case MouseButton::Right:
                return MOUSEEVENTF_RIGHTDOWN;
            case MouseButton::Middle:
                return MOUSEEVENTF_MIDDLEDOWN;
            default:
                return 0;
            }
        }

        inline DWORD mouse_button_to_up_flag(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return MOUSEEVENTF_LEFTUP;
            case MouseButton::Right:
                return MOUSEEVENTF_RIGHTUP;
            case MouseButton::Middle:
                return MOUSEEVENTF_MIDDLEUP;
            default:
                return 0;
            }
        }

#elif defined(CROSSINPUT_LINUX)

        // Linux (X11): Maps CrossInput::KeyCode to X11 KeySym
        inline unsigned long keycode_to_os_code(KeyCode key)
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
                return XK_x; // XK_x
            case KeyCode::KEY_Y:
                return XK_y;
            case KeyCode::KEY_Z:
                return XK_z; // XK_z

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return XK_0; // XK_0
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
                return XK_Shift_L; // Left Shift
            case KeyCode::KEY_CONTROL:
                return XK_Control_L; // Left Control
            case KeyCode::KEY_ALT:
                return XK_Alt_L; // Left Alt
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
                return XK_comma; // ,
            case KeyCode::KEY_PERIOD:
                return XK_period; // .
            case KeyCode::KEY_SEMICOLON:
                return XK_semicolon; // ;
            case KeyCode::KEY_APOSTROPHE:
                return XK_apostrophe; // '
            case KeyCode::KEY_SLASH:
                return XK_slash; // /
            case KeyCode::KEY_BACKSLASH:
                return XK_backslash; // \ 

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

        // X11 Display wrapper for RAII management
        class X11Display
        {
        public:
            X11Display() : display_(XOpenDisplay(nullptr)) {}
            ~X11Display()
            {
                if (display_)
                    XCloseDisplay(display_);
            }

            Display *get() const { return display_; }
            bool isValid() const { return display_ != nullptr; }

            // Non-copyable
            X11Display(const X11Display &) = delete;
            X11Display &operator=(const X11Display &) = delete;

        private:
            Display *display_;
        };

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

#ifdef CROSSINPUT_HAS_LIBEI
        // libei context wrapper for RAII management
        // Uses XDG RemoteDesktop portal to get EIS access
        class LibeiContext
        {
        public:
            LibeiContext() : ei_(nullptr), seat_(nullptr), keyboard_(nullptr), pointer_(nullptr),
                             connection_(nullptr), session_handle_(nullptr), eis_fd_(-1),
                             session_ready_(false), portal_error_(false),
                             keyboard_resumed_(false), pointer_resumed_(false)
            {
                initPortalSession();

                if (ei_)
                {
                    // Process events to get seat and capabilities
                    processEvents();
                }
            }

            ~LibeiContext()
            {
                if (pointer_)
                    ei_device_unref(pointer_);
                if (keyboard_)
                    ei_device_unref(keyboard_);
                if (seat_)
                    ei_seat_unref(seat_);
                if (ei_)
                    ei_unref(ei_);
                if (session_handle_)
                    g_free(session_handle_);
                if (connection_)
                    g_object_unref(connection_);
            }

            bool isValid() const { return ei_ != nullptr && (keyboard_ != nullptr || pointer_ != nullptr); }
            bool hasKeyboard() const { return keyboard_ != nullptr; }
            bool hasPointer() const { return pointer_ != nullptr; }

            ei *get() const { return ei_; }
            ei_device *getKeyboard() const { return keyboard_; }
            ei_device *getPointer() const { return pointer_; }

            void dispatch()
            {
                if (ei_)
                {
                    ei_dispatch(ei_);
                    ei_event *event;
                    while ((event = ei_get_event(ei_)) != nullptr)
                    {
                        handleEvent(event);
                        ei_event_unref(event);
                    }
                }
            }

            // Non-copyable
            LibeiContext(const LibeiContext &) = delete;
            LibeiContext &operator=(const LibeiContext &) = delete;

        private:
            static void onPortalResponse(GDBusConnection *connection,
                                         const gchar *sender_name,
                                         const gchar *object_path,
                                         const gchar *interface_name,
                                         const gchar *signal_name,
                                         GVariant *parameters,
                                         gpointer user_data)
            {
                (void)connection;
                (void)sender_name;
                (void)object_path;
                (void)interface_name;
                (void)signal_name;

                LibeiContext *self = static_cast<LibeiContext *>(user_data);

                guint32 response;
                GVariant *results;
                g_variant_get(parameters, "(u@a{sv})", &response, &results);

                if (response == 0)
                {
                    self->session_ready_ = true;
                }
                else
                {
                    self->portal_error_ = true;
                }

                g_variant_unref(results);
            }

            // Subscribe to signal and return subscription ID
            guint subscribeToResponse(const char *request_path)
            {
                session_ready_ = false;
                portal_error_ = false;

                return g_dbus_connection_signal_subscribe(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "org.freedesktop.portal.Request",
                    "Response",
                    request_path,
                    nullptr,
                    G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
                    onPortalResponse,
                    this,
                    nullptr);
            }

            bool waitForSignal(guint signal_id, int timeout_ms = 30000)
            {
                GMainContext *context = g_main_context_default();
                gint64 end_time = g_get_monotonic_time() + (gint64)(timeout_ms * 1000);

                while (!session_ready_ && !portal_error_)
                {
                    g_main_context_iteration(context, TRUE);

                    if (g_get_monotonic_time() > end_time)
                    {
                        fprintf(stderr, "CrossInput: Portal timeout waiting for response\n");
                        break;
                    }
                }

                g_dbus_connection_signal_unsubscribe(connection_, signal_id);

                return session_ready_;
            }

            void initPortalSession()
            {
                GError *error = nullptr;
                connection_ = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error);
                if (!connection_)
                {
                    fprintf(stderr, "CrossInput: Failed to connect to session bus\n");
                    if (error)
                        g_error_free(error);
                    return;
                }

                // Generate unique tokens
                char token[64];
                snprintf(token, sizeof(token), "crossinput%d", getpid());

                char session_token[64];
                snprintf(session_token, sizeof(session_token), "session%d", getpid());

                // Get sender name for request path
                const char *sender = g_dbus_connection_get_unique_name(connection_);
                char *sender_cleaned = g_strdup(sender + 1);
                for (char *p = sender_cleaned; *p; p++)
                    if (*p == '.')
                        *p = '_';

                // Build expected request path BEFORE making call
                char expected_request_path[512];
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, token);

                // Subscribe to Response BEFORE making the call
                guint signal_id = subscribeToResponse(expected_request_path);

                // === Step 1: CreateSession ===
                GVariantBuilder options;
                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(token));
                g_variant_builder_add(&options, "{sv}", "session_handle_token", g_variant_new_string(session_token));

                GVariant *result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "CreateSession",
                    g_variant_new("(a{sv})", &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: CreateSession failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                if (!waitForSignal(signal_id, 5000))
                {
                    fprintf(stderr, "CrossInput: CreateSession response timeout\n");
                    g_free(sender_cleaned);
                    return;
                }

                // Build session path
                char session_path[512];
                snprintf(session_path, sizeof(session_path),
                         "/org/freedesktop/portal/desktop/session/%s/%s",
                         sender_cleaned, session_token);
                session_handle_ = g_strdup(session_path);

                // === Step 2: SelectDevices ===
                char select_token[64];
                snprintf(select_token, sizeof(select_token), "selectdevices%d", getpid());

                // Build expected request path
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, select_token);

                // Subscribe BEFORE call
                signal_id = subscribeToResponse(expected_request_path);

                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(select_token));
                g_variant_builder_add(&options, "{sv}", "types", g_variant_new_uint32(3)); // keyboard + pointer

                result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "SelectDevices",
                    g_variant_new("(oa{sv})", session_handle_, &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: SelectDevices failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                if (!waitForSignal(signal_id, 5000))
                {
                    fprintf(stderr, "CrossInput: SelectDevices response timeout\n");
                    g_free(sender_cleaned);
                    return;
                }

                // === Step 3: Start (may show permission dialog) ===
                char start_token[64];
                snprintf(start_token, sizeof(start_token), "start%d", getpid());

                // Build expected request path
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, start_token);

                // Subscribe BEFORE call
                signal_id = subscribeToResponse(expected_request_path);

                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(start_token));

                result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "Start",
                    g_variant_new("(osa{sv})", session_handle_, "", &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: Start failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                // Wait longer for Start - user may need to interact with dialog
                if (!waitForSignal(signal_id, 60000))
                {
                    fprintf(stderr, "CrossInput: Start response timeout (user may need to approve)\n");
                    g_free(sender_cleaned);
                    return;
                }

                fprintf(stderr, "CrossInput: Portal session started successfully\n");

                // === Step 4: ConnectToEIS ===
                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));

                GUnixFDList *fd_list = nullptr;
                result = g_dbus_connection_call_with_unix_fd_list_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "ConnectToEIS",
                    g_variant_new("(oa{sv})", session_handle_, &options),
                    G_VARIANT_TYPE("(h)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &fd_list,
                    nullptr,
                    &error);

                g_free(sender_cleaned);

                if (!result || !fd_list)
                {
                    fprintf(stderr, "CrossInput: ConnectToEIS failed\n");
                    if (error)
                    {
                        fprintf(stderr, "CrossInput: Error: %s\n", error->message);
                        g_error_free(error);
                    }
                    return;
                }

                gint32 fd_index;
                g_variant_get(result, "(h)", &fd_index);
                g_variant_unref(result);

                eis_fd_ = g_unix_fd_list_get(fd_list, fd_index, &error);
                g_object_unref(fd_list);

                if (eis_fd_ < 0)
                {
                    fprintf(stderr, "CrossInput: Failed to get EIS fd\n");
                    if (error)
                        g_error_free(error);
                    return;
                }

                fprintf(stderr, "CrossInput: Got EIS fd: %d\n", eis_fd_);

                // Create libei context
                ei_ = ei_new_sender(nullptr);
                if (!ei_)
                {
                    fprintf(stderr, "CrossInput: Failed to create ei context\n");
                    close(eis_fd_);
                    return;
                }

                ei_configure_name(ei_, "CrossInput");

                if (ei_setup_backend_fd(ei_, eis_fd_) != 0)
                {
                    fprintf(stderr, "CrossInput: ei_setup_backend_fd failed\n");
                    ei_unref(ei_);
                    ei_ = nullptr;
                    return;
                }

                fprintf(stderr, "CrossInput: libei context initialized successfully\n");
            }

            void processEvents()
            {
                if (!ei_)
                    return;

                struct pollfd pfd;
                pfd.fd = ei_get_fd(ei_);
                pfd.events = POLLIN;

                // Wait for both devices to be added AND resumed
                for (int i = 0; i < 100 && (!keyboard_resumed_ || !pointer_resumed_); ++i)
                {
                    if (poll(&pfd, 1, 100) > 0)
                    {
                        ei_dispatch(ei_);
                    }

                    ei_event *event;
                    while ((event = ei_get_event(ei_)) != nullptr)
                    {
                        handleEvent(event);
                        ei_event_unref(event);
                    }
                }

                if (keyboard_)
                    fprintf(stderr, "CrossInput: Got keyboard device (resumed: %d)\n", keyboard_resumed_);
                if (pointer_)
                    fprintf(stderr, "CrossInput: Got pointer device (resumed: %d)\n", pointer_resumed_);
            }
            void handleEvent(ei_event *event)
            {
                switch (ei_event_get_type(event))
                {
                case EI_EVENT_SEAT_ADDED:
                    if (!seat_)
                    {
                        seat_ = ei_event_get_seat(event);
                        ei_seat_ref(seat_);
                        ei_seat_bind_capabilities(seat_,
                                                  EI_DEVICE_CAP_KEYBOARD,
                                                  EI_DEVICE_CAP_POINTER,
                                                  EI_DEVICE_CAP_POINTER_ABSOLUTE,
                                                  EI_DEVICE_CAP_BUTTON,
                                                  NULL);
                    }
                    break;
                case EI_EVENT_DEVICE_ADDED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (ei_device_has_capability(device, EI_DEVICE_CAP_KEYBOARD) && !keyboard_)
                    {
                        keyboard_ = device;
                        ei_device_ref(keyboard_);
                    }
                    if ((ei_device_has_capability(device, EI_DEVICE_CAP_POINTER) ||
                         ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE) ||
                         ei_device_has_capability(device, EI_DEVICE_CAP_BUTTON)) &&
                        !pointer_)
                    {
                        pointer_ = device;
                        ei_device_ref(pointer_);
                        fprintf(stderr, "CrossInput: Pointer capabilities: rel=%d abs=%d btn=%d\n",
                                ei_device_has_capability(device, EI_DEVICE_CAP_POINTER),
                                ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE),
                                ei_device_has_capability(device, EI_DEVICE_CAP_BUTTON));

                        // Print region info for absolute pointer
                        if (ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE))
                        {
                            size_t i = 0;
                            struct ei_region *region;
                            while ((region = ei_device_get_region(device, i)) != nullptr)
                            {
                                fprintf(stderr, "CrossInput: Region %zu: x=%u y=%u w=%u h=%u\n",
                                        i, ei_region_get_x(region), ei_region_get_y(region),
                                        ei_region_get_width(region), ei_region_get_height(region));
                                i++;
                            }
                            if (i == 0)
                            {
                                fprintf(stderr, "CrossInput: No regions found for absolute pointer\n");
                            }
                        }
                    }
                    break;
                }
                case EI_EVENT_DEVICE_RESUMED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (device == keyboard_)
                    {
                        keyboard_resumed_ = true;
                        fprintf(stderr, "CrossInput: Keyboard device resumed\n");
                    }
                    if (device == pointer_)
                    {
                        pointer_resumed_ = true;
                        fprintf(stderr, "CrossInput: Pointer device resumed\n");
                    }
                    break;
                }
                case EI_EVENT_DEVICE_PAUSED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (device == keyboard_)
                        keyboard_resumed_ = false;
                    if (device == pointer_)
                        pointer_resumed_ = false;
                    fprintf(stderr, "CrossInput: Device paused\n");
                    break;
                }
                default:
                    break;
                }
            }

            ei *ei_;
            ei_seat *seat_;
            ei_device *keyboard_;
            ei_device *pointer_;
            GDBusConnection *connection_;
            char *session_handle_;
            int eis_fd_;
            bool session_ready_;
            bool portal_error_;
            bool keyboard_resumed_;
            bool pointer_resumed_;
        };
#endif // CROSSINPUT_HAS_LIBEI

#endif // CROSSINPUT_LINUX

    } // namespace Internal
} // namespace CrossInput
