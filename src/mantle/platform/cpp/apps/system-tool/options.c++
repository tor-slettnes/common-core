// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief System service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "system-providers-grpc.h++"
#include "platform/path.h++"
#include "buildinfo.h"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name())
{
    this->describe("Command-line tool for System gRPC service.");
}

void Options::add_options()
{
    Super::add_options();
    this->add_commands();
}

void Options::initialize()
{
    logf_info("Creating FOUP client: %s");
    platform::system::grpc::register_providers(this->host);
    core::platform::signal_shutdown.connect(
        this->signal_handle,
        std::bind(&Options::deinitialize, this));
}

void Options::deinitialize()
{
    core::platform::signal_shutdown.disconnect(this->signal_handle);
    logf_info("Shutting down FOUP client");
    platform::system::grpc::unregister_providers();
}

bool Options::handle_command(const Handler &handler)
{
    this->initialize();
    bool result = Super::handle_command(handler);
    this->deinitialize();
    return result;
}

std::unique_ptr<Options> options;
