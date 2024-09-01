// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief VFS service control tool - command implementations
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "options.h++"
#include "upgrade.h++"
#include "protobuf-message.h++"
#include "string/format.h++"
#include "string/convert.h++"

#include <fstream>
#include <cstdlib>

void Options::add_commands()
{
    this->add_command(
        "scan",
        {"{default | vfs CONTEXT:[PATH] | url URL"},
        "Scan a VFS path or online for upgrade packages. If no source is given, "
        "scan the default download site.",
        std::bind(&Options::scan, this));

    this->add_command(
        "list_available",
        {},
        "List upgrade packages discovered from prior scans.",
        std::bind(&Options::list_available, this));

    this->add_command(
        "best_available",
        {},
        "List upgrade packages discovered from prior scans.",
        std::bind(&Options::best_available, this));

    this->add_command(
        "install",
        {"[PACKAGE]"},

        "Install an software upgrade package. If no package path is provided, "
        "install the best available package discovered from prior scans.",
        std::bind(&Options::install, this));

    this->add_command(
        "finalize",
        {},
        "Finalize a software upgrade.  If the release requires a system reboot, "
        "do so now.",
        std::bind(&Options::finalize, this));

    this->add_command(
        "monitor",
        {"[except]", "[scan_progress|available|upgrade_progress]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));

    this->describe("Upgrade service utility");
}

void Options::scan()
{
    platform::upgrade::PackageSource source;
    std::string arg = this->get_arg("source type");
    if (core::str::tolower(arg) == "vfs")
    {
        source.location = this->vfspath(this->get_arg("vfs path"));
    }
    else if (core::str::tolower(arg) == "url")
    {
        source.location = this->get_arg("url");
    }
    else if (core::str::tolower(arg) != "default")
    {
        std::cerr << "source type bust be one of 'default', 'vfs', 'url'"
                  << std::endl;
        std::exit(-1);
    }

    platform::upgrade::upgrade->scan(source);
}

void Options::list_available()
{
}

void Options::best_available()
{
}

void Options::install()
{
}

void Options::finalize()
{
}

platform::vfs::Path Options::vfspath(const std::string &path)
{
    std::vector<std::string> parts = core::str::split(path, ":", 1, true);
    return {parts.at(0),
            parts.size() > 1 ? parts.at(1) : ""};
}
