#pragma once

#ifdef CROSSINPUT_MACOS

#include "../../../include/CrossInput.h"
#include <Carbon/Carbon.h>

namespace CrossInput
{
    namespace Internal
    {

        // macOS: Maps CrossInput::KeyCode to macOS virtual key codes (CGKeyCode)
        inline CGKeyCode keycode_to_cg(KeyCode key)
        {
            switch (key)
            {
            // Alphanumeric Keys (A-Z)
            case KeyCode::KEY_A:
                return kVK_ANSI_A;
            case KeyCode::KEY_B:
                return kVK_ANSI_B;
            case KeyCode::KEY_C:
                return kVK_ANSI_C;
            case KeyCode::KEY_D:
                return kVK_ANSI_D;
            case KeyCode::KEY_E:
                return kVK_ANSI_E;
            case KeyCode::KEY_F:
                return kVK_ANSI_F;
            case KeyCode::KEY_G:
                return kVK_ANSI_G;
            case KeyCode::KEY_H:
                return kVK_ANSI_H;
            case KeyCode::KEY_I:
                return kVK_ANSI_I;
            case KeyCode::KEY_J:
                return kVK_ANSI_J;
            case KeyCode::KEY_K:
                return kVK_ANSI_K;
            case KeyCode::KEY_L:
                return kVK_ANSI_L;
            case KeyCode::KEY_M:
                return kVK_ANSI_M;
            case KeyCode::KEY_N:
                return kVK_ANSI_N;
            case KeyCode::KEY_O:
                return kVK_ANSI_O;
            case KeyCode::KEY_P:
                return kVK_ANSI_P;
            case KeyCode::KEY_Q:
                return kVK_ANSI_Q;
            case KeyCode::KEY_R:
                return kVK_ANSI_R;
            case KeyCode::KEY_S:
                return kVK_ANSI_S;
            case KeyCode::KEY_T:
                return kVK_ANSI_T;
            case KeyCode::KEY_U:
                return kVK_ANSI_U;
            case KeyCode::KEY_V:
                return kVK_ANSI_V;
            case KeyCode::KEY_W:
                return kVK_ANSI_W;
            case KeyCode::KEY_X:
                return kVK_ANSI_X;
            case KeyCode::KEY_Y:
                return kVK_ANSI_Y;
            case KeyCode::KEY_Z:
                return kVK_ANSI_Z;

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return kVK_ANSI_0;
            case KeyCode::KEY_1:
                return kVK_ANSI_1;
            case KeyCode::KEY_2:
                return kVK_ANSI_2;
            case KeyCode::KEY_3:
                return kVK_ANSI_3;
            case KeyCode::KEY_4:
                return kVK_ANSI_4;
            case KeyCode::KEY_5:
                return kVK_ANSI_5;
            case KeyCode::KEY_6:
                return kVK_ANSI_6;
            case KeyCode::KEY_7:
                return kVK_ANSI_7;
            case KeyCode::KEY_8:
                return kVK_ANSI_8;
            case KeyCode::KEY_9:
                return kVK_ANSI_9;

            // Function Keys (F1-F12)
            case KeyCode::KEY_F1:
                return kVK_F1;
            case KeyCode::KEY_F2:
                return kVK_F2;
            case KeyCode::KEY_F3:
                return kVK_F3;
            case KeyCode::KEY_F4:
                return kVK_F4;
            case KeyCode::KEY_F5:
                return kVK_F5;
            case KeyCode::KEY_F6:
                return kVK_F6;
            case KeyCode::KEY_F7:
                return kVK_F7;
            case KeyCode::KEY_F8:
                return kVK_F8;
            case KeyCode::KEY_F9:
                return kVK_F9;
            case KeyCode::KEY_F10:
                return kVK_F10;
            case KeyCode::KEY_F11:
                return kVK_F11;
            case KeyCode::KEY_F12:
                return kVK_F12;

            // Control Keys
            case KeyCode::KEY_ESCAPE:
                return kVK_Escape;
            case KeyCode::KEY_SPACE:
                return kVK_Space;
            case KeyCode::KEY_ENTER:
                return kVK_Return;
            case KeyCode::KEY_TAB:
                return kVK_Tab;
            case KeyCode::KEY_SHIFT:
                return kVK_Shift;
            case KeyCode::KEY_CONTROL:
                return kVK_Control;
            case KeyCode::KEY_ALT:
                return kVK_Option; // Option is Alt on macOS
            case KeyCode::KEY_CAPS_LOCK:
                return kVK_CapsLock;
            case KeyCode::KEY_BACKSPACE:
                return kVK_Delete; // Delete is Backspace on macOS
            case KeyCode::KEY_DELETE:
                return kVK_ForwardDelete;
            case KeyCode::KEY_INSERT:
                return kVK_Help; // No Insert on Mac, Help is closest

            // Arrow Keys
            case KeyCode::KEY_LEFT:
                return kVK_LeftArrow;
            case KeyCode::KEY_RIGHT:
                return kVK_RightArrow;
            case KeyCode::KEY_UP:
                return kVK_UpArrow;
            case KeyCode::KEY_DOWN:
                return kVK_DownArrow;

            // Symbol Keys
            case KeyCode::KEY_COMMA:
                return kVK_ANSI_Comma;
            case KeyCode::KEY_PERIOD:
                return kVK_ANSI_Period;
            case KeyCode::KEY_SEMICOLON:
                return kVK_ANSI_Semicolon;
            case KeyCode::KEY_APOSTROPHE:
                return kVK_ANSI_Quote;
            case KeyCode::KEY_SLASH:
                return kVK_ANSI_Slash;
            case KeyCode::KEY_BACKSLASH:
                return kVK_ANSI_Backslash;

            default:
                return 0xFFFF; // Invalid keycode
            }
        }

        // Mouse button to CGMouseButton mapping
        inline CGMouseButton mouse_button_to_cg(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return kCGMouseButtonLeft;
            case MouseButton::Right:
                return kCGMouseButtonRight;
            case MouseButton::Middle:
                return kCGMouseButtonCenter;
            default:
                return kCGMouseButtonLeft;
            }
        }

        // Get mouse down event type for button
        inline CGEventType mouse_button_down_event(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return kCGEventLeftMouseDown;
            case MouseButton::Right:
                return kCGEventRightMouseDown;
            case MouseButton::Middle:
                return kCGEventOtherMouseDown;
            default:
                return kCGEventLeftMouseDown;
            }
        }

        // Get mouse up event type for button
        inline CGEventType mouse_button_up_event(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::Left:
                return kCGEventLeftMouseUp;
            case MouseButton::Right:
                return kCGEventRightMouseUp;
            case MouseButton::Middle:
                return kCGEventOtherMouseUp;
            default:
                return kCGEventLeftMouseUp;
            }
        }

    } // namespace Internal
} // namespace CrossInput

#endif // CROSSINPUT_MACOS
