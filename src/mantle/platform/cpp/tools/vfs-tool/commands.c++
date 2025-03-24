// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief VFS service control tool - command implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "vfs.h++"
#include "protobuf-message.h++"
#include "string/format.h++"
#include "string/convert.h++"
#include "vfs-grpc-stream.h++"

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
        "volumeinfo",
        {"CXT[:PATH]"},
        "get information about the mounted volume for a specific path",
        std::bind(&Options::get_volume_info, this));

    this->add_command(
        "fileinfo",
        {"CXT:PATH"},
        "get file information for a specific path",
        std::bind(&Options::get_file_info, this));

    this->add_command(
        "dir",
        {"CXT[:PATH]"},
        "Directory list, with details per entry",
        std::bind(&Options::get_dir, this));

    this->add_command(
        "list",
        {"CXT[:PATH]"},
        "Brief directory list, names only.",
        std::bind(&Options::list, this));

    this->add_command(
        "locate",
        {"CXT[:PATH]", "PATTERN", "..."},
        "Locate files matching PATTERN within a folder",
        std::bind(&Options::locate, this));

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

void Options::get_volume_info()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    std::cout << platform::vfs::get_volume_info(vpath) << std::endl;
}

void Options::get_file_info()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    std::cout << platform::vfs::get_file_info(vpath) << std::endl;
}

void Options::get_dir()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    for (const auto &[path, stats] : platform::vfs::get_directory(vpath))
    {
        std::stringstream ss;
        ss << stats;

        core::str::format(std::cout,
                          "%s\n%s",
                          vpath / path,
                          core::str::wrap(ss.str(), 0, 8));
    }
}

void Options::list()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();

    for (const auto &[path, stats] : platform::vfs::get_directory(vpath))
    {
        std::cout << vpath / path << std::endl;
    }
}

void Options::locate()
{
    platform::vfs::Path vpath = this->get_vfspath_arg();
    std::string mask = this->get_arg("PATTERN(s)");
    core::types::PathList masks = {mask};
    while (auto next_mask = this->next_arg())
    {
        masks.push_back(*next_mask);
    }
    for (const auto &[path, props]: platform::vfs::locate(vpath, masks))
    {
        std::cout << vpath / path << std::endl;
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
    local.exceptions(std::ios::failbit);
    auto remote = platform::vfs::vfs->read_file(remotepath);
    remote->exceptions(std::ios::badbit | std::ios::failbit);

    std::streamsize total = 0;
    uint chunks = 0;

    while (auto chunk = platform::vfs::vfs->read_chunk(*remote))
    {
        chunks++;
        total += chunk->size();
        platform::vfs::vfs->write_chunk(local, chunk.value());
    }

    core::str::format(std::cerr,
                      "Received %d bytes in in %d chunks\n",
                      total,
                      chunks);
}

void Options::upload()
{
    fs::path localpath = this->get_arg("local source path");
    platform::vfs::Path remotepath = this->get_vfspath_arg("remote target VFS path");

    std::ifstream local(localpath, std::ios::in | std::ios::binary);
    auto remote = platform::vfs::vfs->write_file(remotepath);
    remote->exceptions(std::ios::failbit | std::ios::badbit);

    std::streamsize total = 0;
    uint chunks = 0;
    while (auto chunk = platform::vfs::vfs->read_chunk(local))
    {
        chunks++;
        total += chunk->size();
        platform::vfs::vfs->write_chunk(*remote, chunk.value());
    }
    core::str::format(std::cerr,
                      "Sent %d bytes in in %d chunks\n",
                      total,
                      chunks);
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
