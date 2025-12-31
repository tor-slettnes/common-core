// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief VFS service control tool - command line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "vfs-providers-local.h++"
#include "vfs-providers-grpc.h++"
#include "platform/path.h++"
#include "buildinfo.h++"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name()),
      local(false)
{
    this->describe("VFS configuration via gRPC service.");
}

void Options::add_options()
{
    Super::add_options();

    this->add_flag({"--local"},
                   "Use built-in functions, do not connect to platform service",
                   &this->local);

    this->add_commands();
}

void Options::initialize()
{
    logf_debug("Creating VFS client");
    if (this->local)
    {
        vfs::local::register_providers();
    }
    else
    {
        vfs::grpc::register_providers(this->host);
    }
    core::platform::signal_shutdown.connect(
        this->signal_handle,
        std::bind(&Options::deinitialize, this));
}

void Options::deinitialize()
{
    core::platform::signal_shutdown.disconnect(this->signal_handle);
    logf_debug("Shutting down VFS configuration client");
    if (this->local)
    {
        vfs::local::unregister_providers();
    }
    else
    {
        vfs::grpc::unregister_providers();
    }
}

void Options::on_monitor_start()
{
    FlagMap flags;
    bool &except = flags["except"];
    bool &show_context = flags["context"];
    bool &show_context_in_use = flags["context_in_use"];
    this->get_flags(&flags, false);

    if (!show_context && !show_context_in_use)
    {
        except = true;
    }

    using namespace std::placeholders;

    if (show_context != except)
    {
        vfs::signal_context.connect(
            this->signal_handle,
            std::bind(&Options::on_context, this, _1, _2, _3));
    }

    if (show_context_in_use != except)
    {
        vfs::signal_context_in_use.connect(
            this->signal_handle,
            std::bind(&Options::on_context_in_use, this, _1, _2, _3));
    }
}

void Options::on_monitor_end()
{
    vfs::signal_context_in_use.disconnect(this->signal_handle);
    vfs::signal_context.disconnect(this->signal_handle);
}

void Options::on_context(
    core::signal::MappingAction action,
    const std::string &key,
    const vfs::Context::ptr &cxt)
{
    core::str::format(std::cout,
                      "[%.0s] signal_context(%s, %s, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      cxt);
}

void Options::on_context_in_use(
    core::signal::MappingAction action,
    const std::string &key,
    const vfs::Context::ptr &cxt)
{
    core::str::format(std::cout,
                      "[%.0s] signal_context_in_use(%s, %s, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      cxt);
}

std::unique_ptr<Options> options;
