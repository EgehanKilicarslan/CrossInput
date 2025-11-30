#include "../platform/platform_detect.h"

// Stub implementation for unsupported platforms
#if !defined(CROSSINPUT_WINDOWS) && !defined(CROSSINPUT_LINUX)

#include "../../include/CrossInput.h"

namespace CrossInput
{

    bool IsKeyPressed(KeyCode) { return false; }
    void KeyDown(KeyCode) {}
    void KeyUp(KeyCode) {}
    void KeyPress(KeyCode) {}
    void MouseButtonDown(MouseButton) {}
    void MouseButtonUp(MouseButton) {}
    void MouseClick(MouseButton) {}
    Point GetCursorPosition() { return Point{0, 0}; }
    void SetCursorPosition(const Point &) {}
    void MoveCursor(int, int) {}
    std::string GetPlatformName() { return "Unsupported"; }

} // namespace CrossInput

#endif
