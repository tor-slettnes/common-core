// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief System service control tool - command implementations
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "options.h++"
#include "system-providers.h++"
#include "protobuf-message.h++"
#include "string/format.h++"

void Options::add_commands()
{
    this->add_command(
        "get_host_info",
        {},
        "Get general information about the host system",
        std::bind(&Options::get_host_info, this));

    this->add_command(
        "set_host_name",
        {"NAME"},
        "Configure the hostname.",
        std::bind(&Options::set_host_name, this));

    this->add_command(
        "reboot",
        {},
        "Reboot the system",
        std::bind(&Options::reboot, this));

    this->add_command(
        "monitor",
        {"[except]", "[time|time_config|tz_info|tz_config|host_info]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));
}

void Options::get_host_info()
{
    std::cout << platform::system::hostconfig->get_host_info()
              << std::endl;
}

void Options::set_host_name()
{
    std::string hostname = this->get_arg("NAME");
    platform::system::hostconfig->set_host_name(hostname);
}

void Options::reboot()
{
    platform::system::hostconfig->reboot();
}

