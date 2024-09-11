// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief VFS service control tool - command implementations
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "options.h++"
#include "vfs.h++"
#include "protobuf-message.h++"
#include "string/format.h++"
#include "string/convert.h++"

#include <fstream>

void Options::add_commands()
{
    this->add_command(
        "contexts",
        {"[removable]", "[open]", "[verbose]"},
        "list available/removable/open VFS contexts",
        std::bind(&Options::get_contexts, this));

    this->add_command(
        "context",
        {"CXT"},
        "get information about the VFS context CXT",
        std::bind(&Options::get_context, this));

    this->add_command(
        "open",
        {"CXT"},
        "open (increase reference count to) CXT",
        std::bind(&Options::open_context, this));

    this->add_command(
        "close",
        {"CXT"},
        "close (decrease reference count to) CXT",
        std::bind(&Options::close_context, this));

    this->add_command(
        "stats",
        {"CXT:PATH"},
        "get file statistics for a specific path",
        std::bind(&Options::get_stats, this));

    this->add_command(
        "dir",
        {"CXT[:PATH]"},
        "detailed contents of a directory",
        std::bind(&Options::get_dir, this));

    this->add_command(
        "list",
        {"CXT[:PATH]", "[verbose]"},
        "Directory list. By default names only, but \"verbose\" shows additional details.",
        std::bind(&Options::list, this));

    this->add_command(
        "copy",
        {"CXT:SRC", "CXT:TGT"},
        "Copy a file or folder",
        std::bind(&Options::copy, this));

    this->add_command(
        "move",
        {"CXT:SRC", "CXT:TGT"},
        "Copy a file or folder",
        std::bind(&Options::move, this));

    this->add_command(
        "remove",
        {"CXT:PATH", "[force]"},
        "Remove a file or folder",
        std::bind(&Options::remove, this));

    this->add_command(
        "mkdir",
        {"CXT:PATH", "[force]"},
        "Create a folder",
        std::bind(&Options::mkdir, this));

    this->add_command(
        "setattr",
        {"CXT:PATH", "KEY", "VALUE"},
        "Set or update a VFS attribute",
        std::bind(&Options::setattr, this));

    this->add_command(
        "getattr",
        {"CXT:PATH", "[KEY] ..."},
        "Get one or all VFS attributes for a path",
        std::bind(&Options::getattrs, this));

    this->add_command(
        "clearattr",
        {"CXT:PATH"},
        "Remove all attributes for a given path",
        std::bind(&Options::clearattr, this));

    this->add_command(
        "download",
        {"CXT:PATH", "LOCALPATH"},
        "Download a file from server",
        std::bind(&Options::download, this));

    this->add_command(
        "upload",
        {"LOCALPATH", "CXT:PATH"},
        "Upload a file to server",
        std::bind(&Options::upload, this));

    this->add_command(
        "monitor",
        {"[except]", "[context|context_in_use]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));

    this->describe("VFS service utility");
}

void Options::get_contexts()
{
    FlagMap flags;
    bool &removable = flags["removable"];
    bool &open = flags["open"];
    bool &verbose = flags["verbose"];
    this->get_flags(&flags);

    for (const auto &[key, cxt] : platform::vfs::get_contexts(removable, open))
    {
        if (verbose)
        {
            core::str::format(std::cout, "%20s = %s\n", key, cxt);
        }
        else
        {
            std::cout << key << std::endl;
        }
    }
}

void Options::get_context()
{
    std::string cxt_name = this->get_context_arg();
    core::str::format(std::cout, "%s\n", platform::vfs::get_context(cxt_name));
}

void Options::open_context()
{
    std::string cxt_name = this->get_context_arg();
    platform::vfs::open_context(cxt_name);
}

void Options::close_context()
{
    std::string cxt_name = this->get_context_arg();
    platform::vfs::close_context(cxt_name);
}

void Options::get_stats()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    std::cout << platform::vfs::file_stats(vpath) << std::endl;
}

void Options::get_dir()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    for (const auto &[path, stats] : platform::vfs::get_directory(vpath))
    {
        core::str::format(std::cout, "%s:\n\t%s\n", path.string(), stats);
    }
}

void Options::list()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();

    FlagMap flags;
    bool &verbose = flags["verbose"];
    this->get_flags(&flags);

    for (const auto &[path, stats] : platform::vfs::get_directory(vpath))
    {
        if (verbose)
        {
            core::str::format(std::cout, "%s : %s\n", vpath / path, stats);
        }
        else
        {
            std::cout << vpath / path << std::endl;
        }
    }
}

void Options::copy()
{
    platform::vfs::Path src = this->get_vfspath_arg("source VFS path");
    platform::vfs::Path tgt = this->get_vfspath_arg("target VFS path");
    platform::vfs::copy(src, tgt);
}

void Options::move()
{
    platform::vfs::Path src = this->get_vfspath_arg("source VFS path");
    platform::vfs::Path tgt = this->get_vfspath_arg("target VFS path");
    platform::vfs::move(src, tgt);
}

void Options::remove()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    FlagMap flags;
    bool &force = flags["force"];
    this->get_flags(&flags);

    platform::vfs::remove(vpath, force = force);
}

void Options::mkdir()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    FlagMap flags;
    bool &force = flags["force"];
    this->get_flags(&flags);
    platform::vfs::create_folder(vpath, force);
}

void Options::setattr()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    core::types::KeyValueMap attributes = this->get_attributes(true);
    platform::vfs::set_attributes(vpath, attributes);
}

void Options::getattrs()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    core::types::KeyValueMap attributes = platform::vfs::get_attributes(vpath);

    if (this->args.size())
    {
        for (const std::string &name : this->args)
        {
            try
            {
                core::str::format(std::cout, "%20s = %r\n", name, attributes.at(name));
            }
            catch (std::out_of_range)
            {
                core::str::format(std::cout, "%20s is (missing)\n", name);
            }
        }
    }
    else
    {
        for (const auto &[name, value] : attributes)
        {
            core::str::format(std::cout, "%20s = %r\n", name, value);
        }
    }
}

void Options::clearattr()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    platform::vfs::clear_attributes(vpath);
}

void Options::download()
{
    platform::vfs::Path remotepath = this->get_vfspath_arg("remote source VFS path");
    fs::path localpath = this->get_arg("local target path");

    std::ofstream local(localpath, std::ios::out | std::ios::binary);
    local.exceptions(std::ios_base::failbit);
    auto remote = platform::vfs::read_file(remotepath);
    remote->exceptions(std::ios_base::badbit);
    local << remote->rdbuf();
}

void Options::upload()
{
    fs::path localpath = this->get_arg("local source path");
    platform::vfs::Path remotepath = this->get_vfspath_arg("remote target VFS path");

    std::ifstream local(localpath, std::ios::in | std::ios::binary);
    auto remote = platform::vfs::write_file(remotepath);
    *remote << local.rdbuf();
}

std::string Options::get_context_arg(const std::string &what)
{
    return this->get_arg(what);
}

platform::vfs::Path Options::get_vfspath_arg(const std::string &what)
{
    return this->vfspath(this->get_arg(what));
}

platform::vfs::Path Options::vfspath(const std::string &path)
{
    std::vector<std::string> parts = core::str::split(path, ":", 1, true);
    return {parts.at(0),
            parts.size() > 1 ? parts.at(1) : ""};
}
