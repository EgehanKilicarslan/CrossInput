#include "../../platform/platform_detect.h"

#ifdef CROSSINPUT_LINUX

#include "../../../include/CrossInput.h"
#include "linux_keycodes.h"
#include "x11_display.h"
#include <X11/extensions/XTest.h>

namespace CrossInput
{
    namespace X11Impl
    {

        bool IsKeyPressed(KeyCode key)
        {
            Internal::X11Display display;
            if (!display.isValid())
                return false;

            unsigned long keysym = Internal::keycode_to_x11_keysym(key);
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

            unsigned long keysym = Internal::keycode_to_x11_keysym(key);
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

            unsigned long keysym = Internal::keycode_to_x11_keysym(key);
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
} // namespace CrossInput

#endif // CROSSINPUT_LINUX
