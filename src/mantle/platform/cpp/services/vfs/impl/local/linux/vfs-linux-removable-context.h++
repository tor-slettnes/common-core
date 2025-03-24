// -*- c++ -*-
//==============================================================================
/// @file vfs-linux-removable-context.h++
/// @brief VFS service - Removable context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-context.h++"

namespace vfs::local
{
    class RemovableContext : public LocalContext
    {
        using Super = LocalContext;

    public:
        RemovableContext(const std::string &name,
                         const fs::path &root,
                         bool writable,
                         const std::string &preexec,
                         const std::string &postexec,
                         const std::string &title,
                         const std::string &devnode,
                         const std::string &fstype);

    protected:
        void on_open() override;
        void on_close() override;

    protected:
        std::string devnode;
        std::string fstype;
    };
}  // namespace vfs::local
