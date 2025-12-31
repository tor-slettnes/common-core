// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief Nework service control tool - command implementations
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "netconfig.h++"
#include "protobuf-message.h++"
#include "string/format.h++"
#include "string/convert.h++"

void Options::add_commands()
{
    this->add_command(
        "get_state",
        {},
        "print global state",
        std::bind(&Options::get_state, this));

    this->add_command(
        "get_devices",
        {},
        "return a list of available network devices",
        std::bind(&Options::get_devices, this));

    this->add_command(
        "get_connections",
        {},
        "return a list of available network connections",
        std::bind(&Options::get_connections, this));

    this->add_command(
        "remove_connection",
        {"ID"},
        "remove a connection",
        std::bind(&Options::remove_connection, this));

    this->add_command(
        "activate_connection",
        {"ID"},
        "activate a saved connection",
        std::bind(&Options::activate_connection, this));

    this->add_command(
        "get_active_connections",
        {},
        "return a list of active network connections",
        std::bind(&Options::get_active_connections, this));

    this->add_command(
        "request_scan",
        {},
        "trigger a new wireless SSID scan",
        std::bind(&Options::request_scan, this));

    this->add_command(
        "get_aps",
        {},
        "return list of available WiFi access points",
        std::bind(&Options::get_aps, this));

    this->add_command(
        "set_wireless_allowed",
        {"BOOL"},
        "Allow or disallow enabling of WiFi",
        std::bind(&Options::set_wireless_allowed, this));

    this->add_command(
        "set_wireless_enabled",
        {"BOOL"},
        "Enable or disable WiFi radio",
        std::bind(&Options::set_wireless_enabled, this));

    this->add_command(
        "monitor",
        {"[except]", "[global|device|connection|active|accesspoint]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));
}

void Options::get_state()
{
    std::cout << netconfig::network->get_global_data() << std::endl;
}

void Options::get_devices()
{
    for (const auto &kv : netconfig::network->get_devices())
    {
        core::str::format(std::cout, "%10s: %s\n", kv.first, kv.second);
    }
}

void Options::get_connections()
{
    for (const auto &kv : netconfig::network->get_connections())
    {
        core::str::format(std::cout, "%10s: %s\n", kv.first, kv.second);
    }
}

void Options::define_connection()
{
    std::string name = this->get_arg("connection name");
    // TODO: Implement
    std::cerr << "Not yet implemented" << std::endl;
}

void Options::remove_connection()
{
    std::string name = this->get_arg("connection name");
    netconfig::network->remove_connection(name);
}

void Options::activate_connection()
{
    std::string name = this->get_arg("connection name");
    netconfig::network->activate_connection(name);
}

void Options::get_active_connections()
{
    for (const auto &[name, conn] : netconfig::network->get_active_connections())
    {
        core::str::format(std::cout, "%20s: %s\n", name, conn);
    }
}

void Options::request_scan()
{
    netconfig::network->request_scan();
}

void Options::get_aps()
{
    for (const auto &[name, ap] : netconfig::network->get_aps())
    {
        core::str::format(std::cout, "%16s: %s\n", name, ap);
    }
}

void Options::connect_ap()
{
    std::string apname = this->get_arg("access point name");
    std::cerr << "Not yet implemented" << std::endl;
}

void Options::set_wireless_allowed()
{
    std::string allowed = this->get_arg("boolean value");
    netconfig::network->set_wireless_allowed(core::str::convert_to<bool>(allowed));
}

void Options::set_wireless_enabled()
{
    std::string enabled = this->get_arg("boolean value");
    netconfig::network->set_wireless_enabled(core::str::convert_to<bool>(enabled));
}
