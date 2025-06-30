// -*- c++ -*-
//==============================================================================
/// @file init.c++
/// @brief Common initialization tasks
//==============================================================================

#include "init.h++"
#include "providers.h++"
#include "settings/settings.h++"
#include "logging/dispatchers/dispatcher.h++"

#include <locale>
#include <csignal>
#include <cstdlib>

namespace core::application
{
    void emit_shutdown_signal()
    {
        if (!core::platform::signal_shutdown.emitted())
        {
            core::platform::signal_shutdown.emit();
        }
    }

    void shutdown_handler(int signal)
    {
        ::signal(SIGINT, SIG_IGN);
        ::signal(SIGTERM, SIG_IGN);
        emit_shutdown_signal();
    }

    void initialize(int argc, char **argv, const std::optional<std::string> &flavor)
    {
        ::signal(SIGINT, shutdown_handler);
        ::signal(SIGTERM, shutdown_handler);

        std::atexit(deinitialize);

        // Apply system locale for `wstring` conversions
        //std::locale::global(std::locale("C"));

        core::platform::register_providers(argc ? argv[0] : "");
        core::init_settings(flavor);
        core::platform::init_tasks.execute();
    }

    void initialize_daemon(int argc, char **argv)
    {
        initialize(argc, argv, "daemon");
    }

    void deinitialize()
    {
        emit_shutdown_signal();
        core::platform::exit_tasks.execute();
        logging::dispatcher.deinitialize();
        core::platform::unregister_providers();
    }
}  // namespace core::application
