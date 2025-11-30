#pragma once

// Platform detection macros
#ifdef _WIN32
#define CROSSINPUT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__linux__)
#define CROSSINPUT_LINUX
#include <cstdlib>
#include <cstring>
#include <string>
#endif

namespace CrossInput
{
    namespace Internal
    {

#ifdef CROSSINPUT_LINUX
        // Check if Wayland compositor is running
        inline bool IsWayland()
        {
            return std::getenv("WAYLAND_DISPLAY") != nullptr;
        }

        // Check if we're in a Wayland session (even if WAYLAND_DISPLAY is unset)
        inline bool IsWaylandSession()
        {
            const char *session_type = std::getenv("XDG_SESSION_TYPE");
            return session_type && std::string(session_type) == "wayland";
        }

        // Check if X11/XWayland is available for reading state
        inline bool HasX11Display()
        {
            return std::getenv("DISPLAY") != nullptr;
        }
#endif

    } // namespace Internal
} // namespace CrossInput
