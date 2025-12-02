#include "../../platform/platform_detect.h"

#ifdef CROSSINPUT_WINDOWS

#include "../../../include/CrossInput.h"
#include "windows_keycodes.h"

namespace CrossInput
{

    bool IsKeyPressed(KeyCode key)
    {
        int vk = Internal::keycode_to_vk(key);
        if (vk == 0)
            return false;

        // GetAsyncKeyState returns a SHORT with the high bit set if the key is pressed
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    void KeyDown(KeyCode key)
    {
        int vk = Internal::keycode_to_vk(key);
        if (vk == 0)
            return;

        INPUT input = {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = static_cast<WORD>(vk);
        input.ki.dwFlags = 0; // Key down
        SendInput(1, &input, sizeof(INPUT));
    }

    void KeyUp(KeyCode key)
    {
        int vk = Internal::keycode_to_vk(key);
        if (vk == 0)
            return;

        INPUT input = {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = static_cast<WORD>(vk);
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    void KeyPress(KeyCode key)
    {
        KeyDown(key);
        KeyUp(key);
    }

    void KeyCombination(const std::initializer_list<KeyCode> &keys)
    {
        // Press all keys down in order
        for (const auto &key : keys)
        {
            KeyDown(key);
        }

        // Release all keys in reverse order
        for (auto it = std::rbegin(keys); it != std::rend(keys); ++it)
        {
            KeyUp(*it);
        }
    }

    void MouseButtonDown(MouseButton button)
    {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = Internal::mouse_button_to_down_flag(button);
        SendInput(1, &input, sizeof(INPUT));
    }

    void MouseButtonUp(MouseButton button)
    {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = Internal::mouse_button_to_up_flag(button);
        SendInput(1, &input, sizeof(INPUT));
    }

    void MouseClick(MouseButton button)
    {
        MouseButtonDown(button);
        MouseButtonUp(button);
    }

    Point GetCursorPosition()
    {
        POINT pt;
        if (GetCursorPos(&pt))
        {
            return Point{static_cast<int>(pt.x), static_cast<int>(pt.y)};
        }
        return Point{0, 0};
    }

    void SetCursorPosition(const Point &pos)
    {
        SetCursorPos(static_cast<int>(pos.x), static_cast<int>(pos.y));
    }

    void MoveCursor(int dx, int dy)
    {
        Point current = GetCursorPosition();
        SetCursorPosition(Point{current.x + dx, current.y + dy});
    }

    std::string GetPlatformName()
    {
        return "Windows";
    }

} // namespace CrossInput

#endif // CROSSINPUT_WINDOWS
