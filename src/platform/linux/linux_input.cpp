#include "../../platform/platform_detect.h"

#ifdef CROSSINPUT_LINUX

#include "../../../include/CrossInput.h"

// Forward declarations for X11 implementation
namespace CrossInput
{
    namespace X11Impl
    {
        bool IsKeyPressed(KeyCode key);
        void KeyDown(KeyCode key);
        void KeyUp(KeyCode key);
        void MouseButtonDown(MouseButton button);
        void MouseButtonUp(MouseButton button);
        Point GetCursorPosition();
        void SetCursorPosition(const Point &pos);
        void MoveCursor(int dx, int dy);
    } // namespace X11Impl

#ifdef CROSSINPUT_HAS_LIBEI
    namespace WaylandImpl
    {
        void KeyDown(KeyCode key);
        void KeyUp(KeyCode key);
        void MouseButtonDown(MouseButton button);
        void MouseButtonUp(MouseButton button);
        void SetCursorPosition(const Point &pos);
        void MoveCursor(int dx, int dy);
    } // namespace WaylandImpl
#endif

    // --- Public API Implementation (Hybrid approach: X11 for reading state, libei for input) ---

    bool IsKeyPressed(KeyCode key)
    {
        // Hybrid approach: Use X11/XWayland to get key state even on Wayland
        // XWayland provides key state to X11 clients
        if (Internal::HasX11Display())
        {
            return X11Impl::IsKeyPressed(key);
        }

        // Pure Wayland without X11 - cannot get key state
        return false;
    }

    void KeyDown(KeyCode key)
    {
        // Hybrid approach: On Wayland sessions, use libei for input simulation
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::KeyDown(key);
            return;
        }
#endif

        X11Impl::KeyDown(key);
    }

    void KeyUp(KeyCode key)
    {
        // Hybrid approach: On Wayland sessions, use libei for input simulation
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::KeyUp(key);
            return;
        }
#endif

        X11Impl::KeyUp(key);
    }

    void KeyPress(KeyCode key)
    {
        KeyDown(key);
        KeyUp(key);
    }

    void MouseButtonDown(MouseButton button)
    {
        // Hybrid approach: On Wayland sessions, use libei for input simulation
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::MouseButtonDown(button);
            return;
        }
#endif

        X11Impl::MouseButtonDown(button);
    }

    void MouseButtonUp(MouseButton button)
    {
        // Hybrid approach: On Wayland sessions, use libei for input simulation
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::MouseButtonUp(button);
            return;
        }
#endif

        X11Impl::MouseButtonUp(button);
    }

    void MouseClick(MouseButton button)
    {
        MouseButtonDown(button);
        MouseButtonUp(button);
    }

    Point GetCursorPosition()
    {
        // Hybrid approach: Use X11/XWayland to get cursor position even on Wayland
        // This works because XWayland provides cursor position to X11 clients
        if (Internal::HasX11Display())
        {
            return X11Impl::GetCursorPosition();
        }

        // Pure Wayland without X11 - cannot get cursor position
        return Point{0, 0};
    }

    void SetCursorPosition(const Point &pos)
    {
        // Hybrid approach: On Wayland sessions, use libei for cursor movement
        // (XWayland blocks XWarpPointer for security)
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::SetCursorPosition(pos);
            return;
        }
#endif

        X11Impl::SetCursorPosition(pos);
    }

    void MoveCursor(int dx, int dy)
    {
        // Hybrid approach: On Wayland sessions, use libei for cursor movement
#ifdef CROSSINPUT_HAS_LIBEI
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
            WaylandImpl::MoveCursor(dx, dy);
            return;
        }
#endif

        X11Impl::MoveCursor(dx, dy);
    }

    std::string GetPlatformName()
    {
        if (Internal::IsWayland() || Internal::IsWaylandSession())
        {
#ifdef CROSSINPUT_HAS_LIBEI
            return "Linux (Hybrid: Wayland/libei + XWayland)";
#else
            return "Linux (Wayland - No libei support)";
#endif
        }
        return "Linux (X11)";
    }

} // namespace CrossInput

#endif // CROSSINPUT_LINUX
