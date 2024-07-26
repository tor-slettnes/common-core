// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.h++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-context.h++"

namespace platform::vfs
{
    class RemoteContext : public Context
    {
    public:
        using Context::Context;

        void add_ref() override;
        void del_ref() override;
        // fs::path localPath(const fs::path &relpath) const override;
    };
}  // namespace platform::vfs::remote
