// -*- c++ -*-
//==============================================================================
/// @file init.c++
/// @brief Common initialization tasks
//==============================================================================

#include "init.h++"
#include "providers.h++"
#include "platform/path.h++"
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

    void initialize(
        int argc,
        char **argv,
        const std::optional<std::string> &application,
        const std::optional<std::string> &flavor)
    {
        ::signal(SIGINT, shutdown_handler);
        ::signal(SIGTERM, shutdown_handler);

        std::atexit(deinitialize);

        // Apply system locale for `wstring` conversions
        //std::locale::global(std::locale("C"));

        core::platform::register_providers(argc ? argv[0] : "");

        std::string primary_settings_file = application.value_or(
            platform::path->exec_name(true));

        core::init_settings(primary_settings_file, flavor);
        core::platform::init_tasks.execute();
    }

    void initialize_daemon(
        int argc,
        char **argv,
        const std::optional<std::string> &application)
    {
        initialize(argc, argv, application, "daemon");
    }

    void deinitialize()
    {
        emit_shutdown_signal();
        core::platform::exit_tasks.execute();
        logging::dispatcher.deinitialize();
        core::platform::unregister_providers();
    }
}  // namespace core::application
