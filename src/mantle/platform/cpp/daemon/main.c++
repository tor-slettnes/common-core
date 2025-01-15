// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Platform Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"                     // Command-line options
#include "glib-mainloop.h++"               // GLib main loop
#include "platform-grpc-server.h++"        // Load gRPC server
#include "sysconfig-providers-native.h++"  // SysConfig services
#include "netconfig-providers-dbus.h++"    // NetConfig services
#include "vfs-providers-local.h++"         // Virtual Filesystem services
#include "upgrade-providers-native.h++"    // Virtual Filesystem services
#include "application/init.h++"            // Common init routines
#include "status/exceptions.h++"
#include "thread/supervised_thread.h++"

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        core::application::initialize_daemon(argc, argv);
        ::options = std::make_unique<Options>();
        ::options->apply(argc, argv);

        platform::sysconfig::native::register_providers();
        platform::netconfig::dbus::register_providers();
        platform::vfs::local::register_providers();
        platform::upgrade::native::register_providers();

        std::vector<std::thread> server_threads;

        logf_debug("Spawning GLib main loop");
        server_threads.push_back(
            core::thread::supervised_thread(
                core::glib::mainloop));

        logf_debug("Spawning gRPC service");
        server_threads.push_back(
            core::thread::supervised_thread(
                platform::run_grpc_service,
                ::options->bind_address));

        for (std::thread& t : server_threads)
        {
            t.join();
        }

        platform::upgrade::native::unregister_providers();
        platform::vfs::local::unregister_providers();
        platform::netconfig::dbus::unregister_providers();
        platform::sysconfig::native::unregister_providers();

        return 0;
    }
    catch (...)
    {
        log_critical(std::current_exception());
        return -1;
    }
}
