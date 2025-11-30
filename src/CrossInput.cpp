#include "../include/CrossInput.h"
#include "platform_utils.h"

#ifdef CROSSINPUT_LINUX
#include <memory>
#ifdef CROSSINPUT_HAS_LIBEI
#include <poll.h>
#endif
#endif

namespace CrossInput
{

// =============================================================================
// WINDOWS IMPLEMENTATION
// =============================================================================
#ifdef CROSSINPUT_WINDOWS

    bool IsKeyPressed(KeyCode key)
    {
        int vk = Internal::keycode_to_os_code(key);
        if (vk == 0)
            return false;

        // GetAsyncKeyState returns a SHORT with the high bit set if the key is pressed
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    void KeyDown(KeyCode key)
    {
        int vk = Internal::keycode_to_os_code(key);
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
        int vk = Internal::keycode_to_os_code(key);
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

    std::string GetPlatformName()
    {
        return "Windows";
    }

// =============================================================================
// LINUX IMPLEMENTATION (X11 + Wayland/libei)
// =============================================================================
#elif defined(CROSSINPUT_LINUX)

    // --- X11 Implementation Functions ---
    namespace X11Impl
    {
        bool IsKeyPressed(KeyCode key)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return false;

            unsigned long keysym = Internal::keycode_to_os_code(key);
            if (keysym == 0)
                return false;

            unsigned char xKeycode = XKeysymToKeycode(display.get(), keysym);
            if (xKeycode == 0)
                return false;

            char keys[32];
            XQueryKeymap(display.get(), keys);

            return (keys[xKeycode / 8] & (1 << (xKeycode % 8))) != 0;
        }

        void KeyDown(KeyCode key)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            unsigned long keysym = Internal::keycode_to_os_code(key);
            if (keysym == 0)
                return;

            unsigned char xKeycode = XKeysymToKeycode(display.get(), keysym);
            if (xKeycode == 0)
                return;

            XTestFakeKeyEvent(display.get(), xKeycode, True, CurrentTime);
            XFlush(display.get());
        }

        void KeyUp(KeyCode key)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            unsigned long keysym = Internal::keycode_to_os_code(key);
            if (keysym == 0)
                return;

            unsigned char xKeycode = XKeysymToKeycode(display.get(), keysym);
            if (xKeycode == 0)
                return;

            XTestFakeKeyEvent(display.get(), xKeycode, False, CurrentTime);
            XFlush(display.get());
        }

        void MouseButtonDown(MouseButton button)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            unsigned int x11Button = Internal::mouse_button_to_x11_button(button);
            if (x11Button == 0)
                return;

            XTestFakeButtonEvent(display.get(), x11Button, True, CurrentTime);
            XFlush(display.get());
        }

        void MouseButtonUp(MouseButton button)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            unsigned int x11Button = Internal::mouse_button_to_x11_button(button);
            if (x11Button == 0)
                return;

            XTestFakeButtonEvent(display.get(), x11Button, False, CurrentTime);
            XFlush(display.get());
        }

        Point GetCursorPosition()
        {
            Internal::X11Display display;
            if (!display.isValid())
                return Point{0, 0};

            Window root = DefaultRootWindow(display.get());
            Window root_return, child_return;
            int rootX, rootY, winX, winY;
            unsigned int mask;

            if (XQueryPointer(display.get(), root, &root_return, &child_return, &rootX, &rootY, &winX, &winY, &mask))
                return Point{rootX, rootY};

            return Point{0, 0};
        }

        void SetCursorPosition(const Point &pos)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            Window root = DefaultRootWindow(display.get());
            XWarpPointer(display.get(), None, root, 0, 0, 0, 0, pos.x, pos.y);
            XFlush(display.get());
        }

        void MoveCursor(int dx, int dy)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return;

            // XWarpPointer with src_w/src_h = 0 means move relative to current position
            XWarpPointer(display.get(), None, None, 0, 0, 0, 0, dx, dy);
            XFlush(display.get());
        }
    } // namespace X11Impl

#ifdef CROSSINPUT_HAS_LIBEI
    // --- Wayland/libei Implementation Functions ---
    namespace WaylandImpl
    {
        // Thread-local libei context for performance (avoids reconnecting each call)
        // Note: In production, you may want a more sophisticated connection management
        static thread_local std::unique_ptr<Internal::LibeiContext> s_libeiContext;

        Internal::LibeiContext *getContext()
        {
            if (!s_libeiContext || !s_libeiContext->isValid())
            {
                s_libeiContext = std::make_unique<Internal::LibeiContext>();
            }
            return s_libeiContext.get();
        }

        void KeyDown(KeyCode key)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasKeyboard())
                return;

            unsigned int evdevCode = Internal::keycode_to_evdev(key);
            if (evdevCode == 0)
                return;

            ei_device *kbd = ctx->getKeyboard();
            ei_device_start_emulating(kbd, 0);
            ei_device_keyboard_key(kbd, evdevCode, true);
            ei_device_frame(kbd, ei_now(ctx->get()));
            ei_device_stop_emulating(kbd);
            ctx->dispatch();
        }

        void KeyUp(KeyCode key)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasKeyboard())
                return;

            unsigned int evdevCode = Internal::keycode_to_evdev(key);
            if (evdevCode == 0)
                return;

            ei_device *kbd = ctx->getKeyboard();
            ei_device_start_emulating(kbd, 0);
            ei_device_keyboard_key(kbd, evdevCode, false);
            ei_device_frame(kbd, ei_now(ctx->get()));
            ei_device_stop_emulating(kbd);
            ctx->dispatch();
        }

        void MouseButtonDown(MouseButton button)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasPointer())
                return;

            unsigned int evdevButton = Internal::mouse_button_to_evdev(button);
            if (evdevButton == 0)
                return;

            ei_device *ptr = ctx->getPointer();
            ei_device_start_emulating(ptr, 0);
            ei_device_button_button(ptr, evdevButton, true);
            ei_device_frame(ptr, ei_now(ctx->get()));
            ei_device_stop_emulating(ptr);
            ctx->dispatch();
        }

        void MouseButtonUp(MouseButton button)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasPointer())
                return;

            unsigned int evdevButton = Internal::mouse_button_to_evdev(button);
            if (evdevButton == 0)
                return;

            ei_device *ptr = ctx->getPointer();
            ei_device_start_emulating(ptr, 0);
            ei_device_button_button(ptr, evdevButton, false);
            ei_device_frame(ptr, ei_now(ctx->get()));
            ei_device_stop_emulating(ptr);
            ctx->dispatch();
        }

        void SetCursorPosition(const Point &pos)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasPointer())
            {
                fprintf(stderr, "CrossInput: SetCursorPosition - invalid context or no pointer\n");
                return;
            }

            ei_device *ptr = ctx->getPointer();

            // Check if device supports absolute positioning
            if (ei_device_has_capability(ptr, EI_DEVICE_CAP_POINTER_ABSOLUTE))
            {
                fprintf(stderr, "CrossInput: Moving to absolute position (%d, %d)\n", pos.x, pos.y);

                // Get the region to verify coordinates
                struct ei_region *region = ei_device_get_region(ptr, 0);
                if (region)
                {
                    uint32_t rx = ei_region_get_x(region);
                    uint32_t ry = ei_region_get_y(region);
                    uint32_t rw = ei_region_get_width(region);
                    uint32_t rh = ei_region_get_height(region);

                    // Clamp coordinates to region
                    double x = static_cast<double>(pos.x);
                    double y = static_cast<double>(pos.y);

                    if (x < rx)
                        x = rx;
                    if (y < ry)
                        y = ry;
                    if (x >= rx + rw)
                        x = rx + rw - 1;
                    if (y >= ry + rh)
                        y = ry + rh - 1;

                    ei_device_start_emulating(ptr, 0);
                    ei_device_pointer_motion_absolute(ptr, x, y);
                    ei_device_frame(ptr, ei_now(ctx->get()));
                    ei_device_stop_emulating(ptr);

                    // Ensure events are sent
                    int fd = ei_get_fd(ctx->get());
                    struct pollfd pfd = {fd, POLLOUT, 0};
                    poll(&pfd, 1, 10);

                    ctx->dispatch();
                }
                else
                {
                    fprintf(stderr, "CrossInput: No region available\n");
                }
            }
            else if (ei_device_has_capability(ptr, EI_DEVICE_CAP_POINTER))
            {
                fprintf(stderr, "CrossInput: No absolute cap, using relative fallback\n");
                // Relative motion only - we need to track our own position
                static int last_x = 0, last_y = 0;
                static bool initialized = false;

                if (!initialized)
                {
                    // First call - assume we're at some position
                    last_x = pos.x;
                    last_y = pos.y;
                    initialized = true;
                    return;
                }

                int dx = pos.x - last_x;
                int dy = pos.y - last_y;

                if (dx != 0 || dy != 0)
                {
                    ei_device_start_emulating(ptr, 0);
                    ei_device_pointer_motion(ptr,
                                             static_cast<double>(dx),
                                             static_cast<double>(dy));
                    ei_device_frame(ptr, ei_now(ctx->get()));
                    ei_device_stop_emulating(ptr);
                    ctx->dispatch();

                    last_x = pos.x;
                    last_y = pos.y;
                }
            }
            else
            {
                fprintf(stderr, "CrossInput: No pointer capability available!\n");
            }
        }

        // New function for relative mouse movement
        void MoveCursor(int dx, int dy)
        {
            auto *ctx = getContext();
            if (!ctx || !ctx->isValid() || !ctx->hasPointer())
                return;

            ei_device *ptr = ctx->getPointer();

            if (!ei_device_has_capability(ptr, EI_DEVICE_CAP_POINTER))
                return;

            ei_device_start_emulating(ptr, 0);
            ei_device_pointer_motion(ptr,
                                     static_cast<double>(dx),
                                     static_cast<double>(dy));
            ei_device_frame(ptr, ei_now(ctx->get()));
            ei_device_stop_emulating(ptr);
            ctx->dispatch();
        }
    } // namespace WaylandImpl
#endif // CROSSINPUT_HAS_LIBEI

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

// =============================================================================
// UNSUPPORTED PLATFORM STUBS
// =============================================================================
#else

    bool IsKeyPressed(CrossInput::KeyCode) { return false; }
    void KeyDown(CrossInput::KeyCode) {}
    void KeyUp(CrossInput::KeyCode) {}
    void KeyPress(CrossInput::KeyCode) {}
    void MouseButtonDown(MouseButton) {}
    void MouseButtonUp(MouseButton) {}
    void MouseClick(MouseButton) {}
    Point GetCursorPosition() { return Point{0, 0}; }
    void SetCursorPosition(const Point &) {}
    void MoveCursor(int, int) {}
    std::string GetPlatformName() { return "Unsupported"; }

#endif

} // namespace CrossInput
