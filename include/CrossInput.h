#pragma once
#include <string>

namespace CrossInput
{
    // --- ENUMS AND STRUCTS ---

    enum class MouseButton
    {
        Left,
        Right,
        Middle
    };

    // Essential and commonly used key codes, abstracted from the OS.
    enum class KeyCode
    {
        // Alphanumeric
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,

        // Function Keys
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,

        // Control Keys
        KEY_ESCAPE,
        KEY_SPACE,
        KEY_ENTER,
        KEY_TAB,
        KEY_SHIFT,
        KEY_CONTROL,
        KEY_ALT,
        KEY_CAPS_LOCK,
        KEY_BACKSPACE,
        KEY_DELETE,
        KEY_INSERT,

        // Arrow Keys
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN,

        // Symbol Keys
        KEY_COMMA,      // ,
        KEY_PERIOD,     // .
        KEY_SEMICOLON,  // ;
        KEY_APOSTROPHE, // ' or "
        KEY_SLASH,      // / or ?
        KEY_BACKSLASH,  // \ or |
    };

    struct Point
    {
        int x;
        int y;
    };

    // ----------------------------------------------------
    // KEYBOARD ACTIONS
    // ----------------------------------------------------

    // Checks if a specific key is currently pressed globally
    bool IsKeyPressed(KeyCode key);

    // Simulates pressing down a key
    void KeyDown(KeyCode key);
    // Simulates releasing a key
    void KeyUp(KeyCode key);
    // Simulates a full press-release cycle
    void KeyPress(KeyCode key);

    // ----------------------------------------------------
    // MOUSE ACTIONS
    // ----------------------------------------------------

    void MouseButtonDown(MouseButton button);
    void MouseButtonUp(MouseButton button);
    void MouseClick(MouseButton button);

    // Updated to use the Point struct
    Point GetCursorPosition();
    // Updated to use the Point struct
    void SetCursorPosition(const Point &pos);
    // Move cursor by relative amount (works better on Wayland)
    void MoveCursor(int dx, int dy);

    // ----------------------------------------------------
    // SYSTEM INFO
    // ----------------------------------------------------
    std::string GetPlatformName();
}