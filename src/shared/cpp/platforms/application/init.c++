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

namespace shared::application
{
    void shutdown_handler(int signal)
    {
        ::signal(SIGINT, SIG_IGN);
        ::signal(SIGTERM, SIG_IGN);
        shared::platform::signal_shutdown.emit();
    }

    void initialize(int argc, char **argv)
    {
        ::signal(SIGINT, shutdown_handler);
        ::signal(SIGTERM, shutdown_handler);

        // Apply system locale for `wstring` conversions
        std::locale::global(std::locale(""));

        shared::platform::register_providers(argc ? argv[0] : "");
        shared::platform::init_tasks.execute();
    }

    void deinitialize()
    {
        shared::platform::exit_tasks.execute();
        logging::message_dispatcher.deinitialize();
        shared::platform::unregister_providers();
    }
}  // namespace shared::application
