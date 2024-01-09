// -*- c++ -*-
//==============================================================================
/// @file init.c++
/// @brief Common initialization tasks
//==============================================================================

#include "init.h++"
#include "providers.h++"
#include "config/settings.h++"
#include "logging/logging.h++"

#include <locale>
#include <csignal>

namespace cc::application
{
    void shutdown_handler(int signal)
    {
        signal_shutdown.emit(signal);
    }

    void initialize(int argc, char **argv)
    {
        ::signal(SIGINT, shutdown_handler);
        ::signal(SIGTERM, shutdown_handler);

        // Apply locale for `wstring` conversions
        std::locale::global(std::locale(""));

        cc::platform::register_providers(argc ? argv[0] : "");
        cc::init_settings();
    }

    void deinitialize()
    {
        logging::message_dispatcher.deinitialize();
        cc::platform::unregister_providers();
    }

    cc::signal::Signal<int> signal_shutdown("signal_shutdown");
}  // namespace cc::application
