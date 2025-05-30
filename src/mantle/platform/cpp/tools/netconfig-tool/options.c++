// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief NetConfig service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "netconfig-providers-dbus.h++"
#include "netconfig-providers-grpc.h++"
#include "platform/path.h++"
#include "buildinfo.h++"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name()),
      local(false)
{
    this->describe("Network configuration via NetConfig gRPC service.");
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
    logf_debug("Creating NetConfig client: %s");
    if (this->local)
    {
        netconfig::dbus::register_providers();
    }
    else
    {
        netconfig::grpc::register_providers(this->host);
    }
    core::platform::signal_shutdown.connect(
        this->signal_handle,
        std::bind(&Options::deinitialize, this));
}

void Options::deinitialize()
{
    core::platform::signal_shutdown.disconnect(this->signal_handle);
    logf_debug("Shutting down NetConfig client");
    if (this->local)
    {
        netconfig::dbus::unregister_providers();
    }
    else
    {
        netconfig::grpc::unregister_providers();
    }
}

void Options::on_monitor_start()
{
    FlagMap flags;
    bool &except = flags["except"];
    bool &show_global = flags["global"];
    bool &show_connection = flags["connection"];
    bool &show_active = flags["active"];
    bool &show_accesspoint = flags["accesspoint"];
    bool &show_device = flags["device"];
    this->get_flags(&flags, false);

    if (!show_global && !show_device &&
        !show_connection && !show_active &&
        !show_accesspoint)
    {
        except = true;
    }

    using namespace std::placeholders;

    if (show_global != except)
    {
        netconfig::signal_globaldata.connect(
            this->signal_handle,
            std::bind(&Options::on_globaldata, this, _1));
    }

    if (show_connection != except)
    {
        netconfig::signal_connection.connect(
            this->signal_handle,
            std::bind(&Options::on_connection, this, _1, _2, _3));
    }

    if (show_active != except)
    {
        netconfig::signal_active_connection.connect(
            this->signal_handle,
            std::bind(&Options::on_active_connection, this, _1, _2, _3));
    }

    if (show_accesspoint != except)
    {
        netconfig::signal_accesspoint.connect(
            this->signal_handle,
            std::bind(&Options::on_accesspoint, this, _1, _2, _3));
    }

    if (show_device != except)
    {
        netconfig::signal_device.connect(
            this->signal_handle,
            std::bind(&Options::on_device, this, _1, _2, _3));
    }
}

void Options::on_monitor_end()
{
    netconfig::signal_device.disconnect(this->signal_handle);
    netconfig::signal_accesspoint.disconnect(this->signal_handle);
    netconfig::signal_active_connection.disconnect(this->signal_handle);
    netconfig::signal_connection.disconnect(this->signal_handle);
    netconfig::signal_globaldata.disconnect(this->signal_handle);
}

void Options::on_globaldata(
    const netconfig::GlobalData::ptr &data)
{
    core::str::format(std::cout,
                      "[%.0s] signal_globaldata(%s)\n",
                      core::dt::Clock::now(),
                      data);
}

void Options::on_connection(
    core::signal::MappingAction action,
    const std::string &key,
    netconfig::ConnectionData::ptr data)
{
    core::str::format(std::cout,
                      "[%.0s] signal_connection(%s, %r, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      data);
}

void Options::on_active_connection(
    core::signal::MappingAction action,
    const std::string &key,
    netconfig::ActiveConnectionData::ptr data)
{
    core::str::format(std::cout,
                      "[%.0s] signal_active_connection(%s, %r, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      data);
}

void Options::on_accesspoint(
    core::signal::MappingAction action,
    const std::string &key,
    netconfig::AccessPointData::ptr data)
{
    core::str::format(std::cout,
                      "[%.0s] signal_accesspoint(%s, %r, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      data);
}

void Options::on_device(
    core::signal::MappingAction action,
    const std::string &key,
    netconfig::DeviceData::ptr data)
{
    core::str::format(std::cout,
                      "[%.0s] signal_device(%s, %r, %s)\n",
                      core::dt::Clock::now(),
                      action,
                      key,
                      data);
}

std::unique_ptr<Options> options;
