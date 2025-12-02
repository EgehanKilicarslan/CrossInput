#include "../../platform/platform_detect.h"

#ifdef CROSSINPUT_MACOS

#include "../../../include/CrossInput.h"
#include "macos_keycodes.h"
#include <ApplicationServices/ApplicationServices.h>

namespace CrossInput
{

    bool IsKeyPressed(KeyCode key)
    {
        CGKeyCode cgKey = Internal::keycode_to_cg(key);
        if (cgKey == 0xFFFF)
            return false;

        return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, cgKey);
    }

    void KeyDown(KeyCode key)
    {
        CGKeyCode cgKey = Internal::keycode_to_cg(key);
        if (cgKey == 0xFFFF)
            return;

        CGEventRef event = CGEventCreateKeyboardEvent(nullptr, cgKey, true);
        if (event)
        {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
        }
    }

    void KeyUp(KeyCode key)
    {
        CGKeyCode cgKey = Internal::keycode_to_cg(key);
        if (cgKey == 0xFFFF)
            return;

        CGEventRef event = CGEventCreateKeyboardEvent(nullptr, cgKey, false);
        if (event)
        {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
        }
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
        CGPoint location = CGEventGetLocation(CGEventCreate(nullptr));
        CGEventType eventType = Internal::mouse_button_down_event(button);
        CGMouseButton cgButton = Internal::mouse_button_to_cg(button);

        CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, location, cgButton);
        if (event)
        {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
        }
    }

    void MouseButtonUp(MouseButton button)
    {
        CGPoint location = CGEventGetLocation(CGEventCreate(nullptr));
        CGEventType eventType = Internal::mouse_button_up_event(button);
        CGMouseButton cgButton = Internal::mouse_button_to_cg(button);

        CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, location, cgButton);
        if (event)
        {
            CGEventPost(kCGHIDEventTap, event);
            CFRelease(event);
        }
    }

    void MouseClick(MouseButton button)
    {
        MouseButtonDown(button);
        MouseButtonUp(button);
    }

    Point GetCursorPosition()
    {
        CGEventRef event = CGEventCreate(nullptr);
        CGPoint location = CGEventGetLocation(event);
        CFRelease(event);

        return Point{static_cast<int>(location.x), static_cast<int>(location.y)};
    }

    void SetCursorPosition(const Point &pos)
    {
        CGPoint location = CGPointMake(static_cast<CGFloat>(pos.x), static_cast<CGFloat>(pos.y));
        CGWarpMouseCursorPosition(location);

        // Associate mouse with display to ensure cursor updates immediately
        CGAssociateMouseAndMouseCursorPosition(true);
    }

    void MoveCursor(int dx, int dy)
    {
        Point current = GetCursorPosition();
        SetCursorPosition(Point{current.x + dx, current.y + dy});
    }

    std::string GetPlatformName()
    {
        return "macOS";
    }

} // namespace CrossInput

#endif // CROSSINPUT_MACOS
