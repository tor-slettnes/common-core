// -*- c++ -*-
//==============================================================================
/// @file init.c++
/// @brief Common initialization tasks
//==============================================================================

#include "init.h++"
#include "providers.h++"
#include "settings/settings.h++"
#include "logging/logging.h++"

#include <locale>
#include <csignal>
#include <cstdlib>

namespace cc::application
{
    void shutdown_handler(int signal)
    {
        ::signal(SIGINT, SIG_IGN);
        ::signal(SIGTERM, SIG_IGN);
        cc::platform::signal_shutdown.emit();
    }

    void initialize(int argc, char **argv)
    {
        ::signal(SIGINT, shutdown_handler);
        ::signal(SIGTERM, shutdown_handler);

        std::atexit(deinitialize);

        // Apply system locale for `wstring` conversions
        //std::locale::global(std::locale("C"));

        cc::platform::register_providers(argc ? argv[0] : "");
        cc::platform::init_tasks.execute();
    }

    void deinitialize()
    {
        cc::platform::exit_tasks.execute();
        logging::message_dispatcher.deinitialize();
        cc::platform::unregister_providers();
    }
}  // namespace cc::application
