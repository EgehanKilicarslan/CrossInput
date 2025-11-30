#include "../../platform/platform_detect.h"

#ifdef CROSSINPUT_LINUX
#ifdef CROSSINPUT_HAS_LIBEI

#include "../../../include/CrossInput.h"
#include "linux_keycodes.h"
#include "libei_context.h"
#include <memory>
#include <poll.h>
#include <cstdio>

namespace CrossInput
{
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

            // Try relative movement first
            if (ei_device_has_capability(ptr, EI_DEVICE_CAP_POINTER))
            {
                ei_device_start_emulating(ptr, 0);
                ei_device_pointer_motion(ptr,
                                         static_cast<double>(dx),
                                         static_cast<double>(dy));
                ei_device_frame(ptr, ei_now(ctx->get()));
                ei_device_stop_emulating(ptr);
                ctx->dispatch();
                return;
            }

            // Fall back to absolute positioning if only that's available
            if (ei_device_has_capability(ptr, EI_DEVICE_CAP_POINTER_ABSOLUTE))
            {
                struct ei_region *region = ei_device_get_region(ptr, 0);
                if (!region)
                    return;

                // We need to track position ourselves for relative movement
                static double current_x = -1, current_y = -1;

                // Initialize to center of region if not set
                if (current_x < 0)
                {
                    uint32_t rw = ei_region_get_width(region);
                    uint32_t rh = ei_region_get_height(region);
                    current_x = rw / 2.0;
                    current_y = rh / 2.0;
                }

                // Calculate new position
                current_x += dx;
                current_y += dy;

                // Clamp to region bounds
                uint32_t rx = ei_region_get_x(region);
                uint32_t ry = ei_region_get_y(region);
                uint32_t rw = ei_region_get_width(region);
                uint32_t rh = ei_region_get_height(region);

                if (current_x < rx)
                    current_x = rx;
                if (current_y < ry)
                    current_y = ry;
                if (current_x >= rx + rw)
                    current_x = rx + rw - 1;
                if (current_y >= ry + rh)
                    current_y = ry + rh - 1;

                ei_device_start_emulating(ptr, 0);
                ei_device_pointer_motion_absolute(ptr, current_x, current_y);
                ei_device_frame(ptr, ei_now(ctx->get()));
                ei_device_stop_emulating(ptr);
                ctx->dispatch();
            }
        }

    } // namespace WaylandImpl
} // namespace CrossInput

#endif // CROSSINPUT_HAS_LIBEI
#endif // CROSSINPUT_LINUX
