// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief VFS service control tool - command implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "upgrade.h++"
#include "vfs.h++"
#include "sysconfig.h++"
#include "protobuf-message.h++"
#include "string/format.h++"
#include "string/convert.h++"

#include <fstream>
#include <cstdlib>

void Options::add_commands()
{
    this->add_command(
        "get_current",
        {},
        "Return currently installed release info.",
        std::bind(&Options::get_current, this));

    this->add_command(
        "scan",
        {"{default | vfs CONTEXT:[PATH] | url URL}"},
        "Scan a VFS path or online for upgrade packages. If no source is given, "
        "scan the default download site.",
        std::bind(&Options::scan, this));

    this->add_command(
        "list_sources",
        {},
        "List upgrade packages discovered from prior scans.",
        std::bind(&Options::list_sources, this));

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
        {"[{default | file LOCAL_PACKAGE | vfs CONTEXT:PATH | url URL}]"},
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
        {"[except]", "[scan_progress|available|pending|upgrade_progress]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));

    this->describe("Upgrade service utility");
}

void Options::get_current()
{
    std::cout << sysconfig::product->get_product_info() << std::endl;
}

void Options::scan()
{
    upgrade::PackageSource source;
    std::string arg = this->get_arg("source type");
    if (core::str::tolower(arg) == "vfs")
    {
        source.location = this->vfspath(this->next_arg().value_or(""));
    }
    else if (core::str::tolower(arg) == "url")
    {
        source.location = this->get_arg("URL");
    }
    else if (core::str::tolower(arg) != "default")
    {
        std::cerr << "source type bust be one of 'default', 'vfs', 'url'"
                  << std::endl;
        std::exit(-1);
    }

    for (const auto &src : upgrade::upgrade->scan(source))
    {
        std::cout << src << std::endl;
    }
}

void Options::list_sources()
{
    for (const auto &src : upgrade::upgrade->list_sources())
    {
        std::cout << src << std::endl;
    }
}

void Options::list_available()
{
    for (const auto &package_info : upgrade::upgrade->list_available())
    {
        std::cout << *package_info << std::endl;
    }
}

void Options::best_available()
{
    std::cout << *upgrade::upgrade->best_available() << std::endl;
}

void Options::install()
{
    upgrade::PackageSource source;

    if (std::optional<std::string> arg = this->next_arg())
    {
        if (core::str::tolower(arg.value()) == "vfs")
        {
            source.location = this->vfspath(this->get_arg("vfs path"));
        }
        else if (core::str::tolower(arg.value()) == "url")
        {
            source.location = this->get_arg("url");
        }
        else if (core::str::tolower(arg.value()) == "file")
        {
            fs::path local_path(this->get_arg("filename"));
            vfs::Path remote_path("releases", local_path.filename());
            vfs::upload(local_path, remote_path);

            source.location = remote_path;
        }
        else if (core::str::tolower(arg.value()) != "default")
        {
            std::cerr << "source type bust be one of 'default', 'vfs', 'url'"
                      << std::endl;
            std::exit(-1);
        }
    }

    std::cout << *upgrade::upgrade->install(source) << std::endl;
}

void Options::finalize()
{
    upgrade::upgrade->finalize();
}

vfs::Path Options::vfspath(const std::string &path)
{
    std::vector<std::string> parts = core::str::split(path, ":", 1, true);
    return {parts.at(0),
            parts.size() > 1 ? parts.at(1) : ""};
}
