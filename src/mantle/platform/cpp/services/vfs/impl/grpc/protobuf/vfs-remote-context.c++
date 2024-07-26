// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.c++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-remote-context.h++"
#include "vfs.h++"

namespace platform::vfs
{
    void RemoteContext::add_ref()
    {
        vfs::openContext(this->name);
    }

    void RemoteContext::del_ref()
    {
        vfs::closeContext(this->name);
    }
}  // namespace platform::vfs::grpc
