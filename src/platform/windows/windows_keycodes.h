#pragma once

#ifdef CROSSINPUT_WINDOWS

#include "../../../include/CrossInput.h"
#include <windows.h>

namespace CrossInput
{
    namespace Internal
    {

        // Windows: Maps CrossInput::KeyCode to Windows Virtual Key Code (VK_CODE)
        inline int keycode_to_vk(KeyCode key)
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
                return 0x58;
            case KeyCode::KEY_Y:
                return 0x59;
            case KeyCode::KEY_Z:
                return 0x5A;

            // Number Keys (0-9)
            case KeyCode::KEY_0:
                return 0x30;
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
                return VK_LSHIFT;
            case KeyCode::KEY_CONTROL:
                return VK_LCONTROL;
            case KeyCode::KEY_ALT:
                return VK_LMENU;
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
                return VK_OEM_COMMA;
            case KeyCode::KEY_PERIOD:
                return VK_OEM_PERIOD;
            case KeyCode::KEY_SEMICOLON:
                return VK_OEM_1;
            case KeyCode::KEY_APOSTROPHE:
                return VK_OEM_7;
            case KeyCode::KEY_SLASH:
                return VK_OEM_2;
            case KeyCode::KEY_BACKSLASH:
                return VK_OEM_5;

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

    } // namespace Internal
} // namespace CrossInput

#endif // CROSSINPUT_WINDOWS
