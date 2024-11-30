// -*- c++ -*-
//==============================================================================
/// @file vfs-providers-linux.c++
/// @brief VFS service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-providers-local.h++"
#include "vfs-linux-provider.h++"

namespace platform::vfs::local
{
    void register_providers()
    {
        vfs.registerProvider<LinuxProvider>();
    }

    void unregister_providers()
    {
        vfs.unregisterProvider<LinuxProvider>();
    }

}  // namespace platform::vfs::local
