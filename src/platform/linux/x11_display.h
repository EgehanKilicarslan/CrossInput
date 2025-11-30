#pragma once

#ifdef CROSSINPUT_LINUX

#include <X11/Xlib.h>

namespace CrossInput
{
    namespace Internal
    {

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

    } // namespace Internal
} // namespace CrossInput

#endif // CROSSINPUT_LINUX
