#pragma once

#ifdef CROSSINPUT_LINUX
#ifdef CROSSINPUT_HAS_LIBEI

#include <libei.h>
#include <poll.h>
#include <unistd.h>
#include <gio/gio.h>
#include <gio/gunixfdlist.h>
#include <cstdio>

namespace CrossInput
{
    namespace Internal
    {

        // libei context wrapper for RAII management
        // Uses XDG RemoteDesktop portal to get EIS access
        class LibeiContext
        {
        public:
            LibeiContext() : ei_(nullptr), seat_(nullptr), keyboard_(nullptr), pointer_(nullptr),
                             connection_(nullptr), session_handle_(nullptr), eis_fd_(-1),
                             session_ready_(false), portal_error_(false),
                             keyboard_resumed_(false), pointer_resumed_(false)
            {
                initPortalSession();

                if (ei_)
                {
                    // Process events to get seat and capabilities
                    processEvents();
                }
            }

            ~LibeiContext()
            {
                if (pointer_)
                    ei_device_unref(pointer_);
                if (keyboard_)
                    ei_device_unref(keyboard_);
                if (seat_)
                    ei_seat_unref(seat_);
                if (ei_)
                    ei_unref(ei_);
                if (session_handle_)
                    g_free(session_handle_);
                if (connection_)
                    g_object_unref(connection_);
            }

            bool isValid() const { return ei_ != nullptr && (keyboard_ != nullptr || pointer_ != nullptr); }
            bool hasKeyboard() const { return keyboard_ != nullptr; }
            bool hasPointer() const { return pointer_ != nullptr; }

            ei *get() const { return ei_; }
            ei_device *getKeyboard() const { return keyboard_; }
            ei_device *getPointer() const { return pointer_; }

            void dispatch()
            {
                if (ei_)
                {
                    ei_dispatch(ei_);
                    ei_event *event;
                    while ((event = ei_get_event(ei_)) != nullptr)
                    {
                        handleEvent(event);
                        ei_event_unref(event);
                    }
                }
            }

            // Non-copyable
            LibeiContext(const LibeiContext &) = delete;
            LibeiContext &operator=(const LibeiContext &) = delete;

        private:
            static void onPortalResponse(GDBusConnection *connection,
                                         const gchar *sender_name,
                                         const gchar *object_path,
                                         const gchar *interface_name,
                                         const gchar *signal_name,
                                         GVariant *parameters,
                                         gpointer user_data)
            {
                (void)connection;
                (void)sender_name;
                (void)object_path;
                (void)interface_name;
                (void)signal_name;

                LibeiContext *self = static_cast<LibeiContext *>(user_data);

                guint32 response;
                GVariant *results;
                g_variant_get(parameters, "(u@a{sv})", &response, &results);

                if (response == 0)
                {
                    self->session_ready_ = true;
                }
                else
                {
                    self->portal_error_ = true;
                }

                g_variant_unref(results);
            }

            // Subscribe to signal and return subscription ID
            guint subscribeToResponse(const char *request_path)
            {
                session_ready_ = false;
                portal_error_ = false;

                return g_dbus_connection_signal_subscribe(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "org.freedesktop.portal.Request",
                    "Response",
                    request_path,
                    nullptr,
                    G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
                    onPortalResponse,
                    this,
                    nullptr);
            }

            bool waitForSignal(guint signal_id, int timeout_ms = 30000)
            {
                GMainContext *context = g_main_context_default();
                gint64 end_time = g_get_monotonic_time() + (gint64)(timeout_ms * 1000);

                while (!session_ready_ && !portal_error_)
                {
                    g_main_context_iteration(context, TRUE);

                    if (g_get_monotonic_time() > end_time)
                    {
                        fprintf(stderr, "CrossInput: Portal timeout waiting for response\n");
                        break;
                    }
                }

                g_dbus_connection_signal_unsubscribe(connection_, signal_id);

                return session_ready_;
            }

            void initPortalSession()
            {
                GError *error = nullptr;
                connection_ = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error);
                if (!connection_)
                {
                    fprintf(stderr, "CrossInput: Failed to connect to session bus\n");
                    if (error)
                        g_error_free(error);
                    return;
                }

                // Generate unique tokens
                char token[64];
                snprintf(token, sizeof(token), "crossinput%d", getpid());

                char session_token[64];
                snprintf(session_token, sizeof(session_token), "session%d", getpid());

                // Get sender name for request path
                const char *sender = g_dbus_connection_get_unique_name(connection_);
                char *sender_cleaned = g_strdup(sender + 1);
                for (char *p = sender_cleaned; *p; p++)
                    if (*p == '.')
                        *p = '_';

                // Build expected request path BEFORE making call
                char expected_request_path[512];
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, token);

                // Subscribe to Response BEFORE making the call
                guint signal_id = subscribeToResponse(expected_request_path);

                // === Step 1: CreateSession ===
                GVariantBuilder options;
                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(token));
                g_variant_builder_add(&options, "{sv}", "session_handle_token", g_variant_new_string(session_token));

                GVariant *result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "CreateSession",
                    g_variant_new("(a{sv})", &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: CreateSession failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                if (!waitForSignal(signal_id, 5000))
                {
                    fprintf(stderr, "CrossInput: CreateSession response timeout\n");
                    g_free(sender_cleaned);
                    return;
                }

                // Build session path
                char session_path[512];
                snprintf(session_path, sizeof(session_path),
                         "/org/freedesktop/portal/desktop/session/%s/%s",
                         sender_cleaned, session_token);
                session_handle_ = g_strdup(session_path);

                // === Step 2: SelectDevices ===
                char select_token[64];
                snprintf(select_token, sizeof(select_token), "selectdevices%d", getpid());

                // Build expected request path
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, select_token);

                // Subscribe BEFORE call
                signal_id = subscribeToResponse(expected_request_path);

                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(select_token));
                g_variant_builder_add(&options, "{sv}", "types", g_variant_new_uint32(3)); // keyboard + pointer

                result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "SelectDevices",
                    g_variant_new("(oa{sv})", session_handle_, &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: SelectDevices failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                if (!waitForSignal(signal_id, 5000))
                {
                    fprintf(stderr, "CrossInput: SelectDevices response timeout\n");
                    g_free(sender_cleaned);
                    return;
                }

                // === Step 3: Start (may show permission dialog) ===
                char start_token[64];
                snprintf(start_token, sizeof(start_token), "start%d", getpid());

                // Build expected request path
                snprintf(expected_request_path, sizeof(expected_request_path),
                         "/org/freedesktop/portal/desktop/request/%s/%s",
                         sender_cleaned, start_token);

                // Subscribe BEFORE call
                signal_id = subscribeToResponse(expected_request_path);

                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));
                g_variant_builder_add(&options, "{sv}", "handle_token", g_variant_new_string(start_token));

                result = g_dbus_connection_call_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "Start",
                    g_variant_new("(osa{sv})", session_handle_, "", &options),
                    G_VARIANT_TYPE("(o)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &error);

                if (!result)
                {
                    fprintf(stderr, "CrossInput: Start failed\n");
                    g_dbus_connection_signal_unsubscribe(connection_, signal_id);
                    g_free(sender_cleaned);
                    if (error)
                        g_error_free(error);
                    return;
                }
                g_variant_unref(result);

                // Wait longer for Start - user may need to interact with dialog
                if (!waitForSignal(signal_id, 60000))
                {
                    fprintf(stderr, "CrossInput: Start response timeout (user may need to approve)\n");
                    g_free(sender_cleaned);
                    return;
                }

                fprintf(stderr, "CrossInput: Portal session started successfully\n");

                // === Step 4: ConnectToEIS ===
                g_variant_builder_init(&options, G_VARIANT_TYPE("a{sv}"));

                GUnixFDList *fd_list = nullptr;
                result = g_dbus_connection_call_with_unix_fd_list_sync(
                    connection_,
                    "org.freedesktop.portal.Desktop",
                    "/org/freedesktop/portal/desktop",
                    "org.freedesktop.portal.RemoteDesktop",
                    "ConnectToEIS",
                    g_variant_new("(oa{sv})", session_handle_, &options),
                    G_VARIANT_TYPE("(h)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    nullptr,
                    &fd_list,
                    nullptr,
                    &error);

                g_free(sender_cleaned);

                if (!result || !fd_list)
                {
                    fprintf(stderr, "CrossInput: ConnectToEIS failed\n");
                    if (error)
                    {
                        fprintf(stderr, "CrossInput: Error: %s\n", error->message);
                        g_error_free(error);
                    }
                    return;
                }

                gint32 fd_index;
                g_variant_get(result, "(h)", &fd_index);
                g_variant_unref(result);

                eis_fd_ = g_unix_fd_list_get(fd_list, fd_index, &error);
                g_object_unref(fd_list);

                if (eis_fd_ < 0)
                {
                    fprintf(stderr, "CrossInput: Failed to get EIS fd\n");
                    if (error)
                        g_error_free(error);
                    return;
                }

                fprintf(stderr, "CrossInput: Got EIS fd: %d\n", eis_fd_);

                // Create libei context
                ei_ = ei_new_sender(nullptr);
                if (!ei_)
                {
                    fprintf(stderr, "CrossInput: Failed to create ei context\n");
                    close(eis_fd_);
                    return;
                }

                ei_configure_name(ei_, "CrossInput");

                if (ei_setup_backend_fd(ei_, eis_fd_) != 0)
                {
                    fprintf(stderr, "CrossInput: ei_setup_backend_fd failed\n");
                    ei_unref(ei_);
                    ei_ = nullptr;
                    return;
                }

                fprintf(stderr, "CrossInput: libei context initialized successfully\n");
            }

            void processEvents()
            {
                if (!ei_)
                    return;

                struct pollfd pfd;
                pfd.fd = ei_get_fd(ei_);
                pfd.events = POLLIN;

                // Wait for both devices to be added AND resumed
                for (int i = 0; i < 100 && (!keyboard_resumed_ || !pointer_resumed_); ++i)
                {
                    if (poll(&pfd, 1, 100) > 0)
                    {
                        ei_dispatch(ei_);
                    }

                    ei_event *event;
                    while ((event = ei_get_event(ei_)) != nullptr)
                    {
                        handleEvent(event);
                        ei_event_unref(event);
                    }
                }

                if (keyboard_)
                    fprintf(stderr, "CrossInput: Got keyboard device (resumed: %d)\n", keyboard_resumed_);
                if (pointer_)
                    fprintf(stderr, "CrossInput: Got pointer device (resumed: %d)\n", pointer_resumed_);
            }

            void handleEvent(ei_event *event)
            {
                switch (ei_event_get_type(event))
                {
                case EI_EVENT_SEAT_ADDED:
                    if (!seat_)
                    {
                        seat_ = ei_event_get_seat(event);
                        ei_seat_ref(seat_);
                        ei_seat_bind_capabilities(seat_,
                                                  EI_DEVICE_CAP_KEYBOARD,
                                                  EI_DEVICE_CAP_POINTER,
                                                  EI_DEVICE_CAP_POINTER_ABSOLUTE,
                                                  EI_DEVICE_CAP_BUTTON,
                                                  NULL);
                    }
                    break;
                case EI_EVENT_DEVICE_ADDED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (ei_device_has_capability(device, EI_DEVICE_CAP_KEYBOARD) && !keyboard_)
                    {
                        keyboard_ = device;
                        ei_device_ref(keyboard_);
                    }
                    if ((ei_device_has_capability(device, EI_DEVICE_CAP_POINTER) ||
                         ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE) ||
                         ei_device_has_capability(device, EI_DEVICE_CAP_BUTTON)) &&
                        !pointer_)
                    {
                        pointer_ = device;
                        ei_device_ref(pointer_);
                        fprintf(stderr, "CrossInput: Pointer capabilities: rel=%d abs=%d btn=%d\n",
                                ei_device_has_capability(device, EI_DEVICE_CAP_POINTER),
                                ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE),
                                ei_device_has_capability(device, EI_DEVICE_CAP_BUTTON));

                        // Print region info for absolute pointer
                        if (ei_device_has_capability(device, EI_DEVICE_CAP_POINTER_ABSOLUTE))
                        {
                            size_t i = 0;
                            struct ei_region *region;
                            while ((region = ei_device_get_region(device, i)) != nullptr)
                            {
                                fprintf(stderr, "CrossInput: Region %zu: x=%u y=%u w=%u h=%u\n",
                                        i, ei_region_get_x(region), ei_region_get_y(region),
                                        ei_region_get_width(region), ei_region_get_height(region));
                                i++;
                            }
                            if (i == 0)
                            {
                                fprintf(stderr, "CrossInput: No regions found for absolute pointer\n");
                            }
                        }
                    }
                    break;
                }
                case EI_EVENT_DEVICE_RESUMED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (device == keyboard_)
                    {
                        keyboard_resumed_ = true;
                        fprintf(stderr, "CrossInput: Keyboard device resumed\n");
                    }
                    if (device == pointer_)
                    {
                        pointer_resumed_ = true;
                        fprintf(stderr, "CrossInput: Pointer device resumed\n");
                    }
                    break;
                }
                case EI_EVENT_DEVICE_PAUSED:
                {
                    ei_device *device = ei_event_get_device(event);
                    if (device == keyboard_)
                        keyboard_resumed_ = false;
                    if (device == pointer_)
                        pointer_resumed_ = false;
                    fprintf(stderr, "CrossInput: Device paused\n");
                    break;
                }
                default:
                    break;
                }
            }

            ei *ei_;
            ei_seat *seat_;
            ei_device *keyboard_;
            ei_device *pointer_;
            GDBusConnection *connection_;
            char *session_handle_;
            int eis_fd_;
            bool session_ready_;
            bool portal_error_;
            bool keyboard_resumed_;
            bool pointer_resumed_;
        };

    } // namespace Internal
} // namespace CrossInput

#endif // CROSSINPUT_HAS_LIBEI
#endif // CROSSINPUT_LINUX
