// -*- c++ -*-
//==============================================================================
/// @file glib-mainloop.c++
/// @brief GLib main loop
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "glib-mainloop.h++"
#include "logging/logging.h++"
#include "platform/init.h++"

#include <glibmm/main.h>
#include <functional>

namespace core::glib
{
    void mainloop()
    {
        log_debug("Adding shutdown handler for GLib main loop");
        auto mainloop = Glib::MainLoop::create();

        std::string signal_handle = core::platform::signal_shutdown.connect(
            std::bind(&Glib::MainLoop::quit, mainloop.get()));

        mainloop->run();

        core::platform::signal_shutdown.disconnect(signal_handle);
    }
}  // namespace core::glib
