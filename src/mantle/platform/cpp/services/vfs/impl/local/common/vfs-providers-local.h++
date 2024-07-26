// -*- c++ -*-
//==============================================================================
/// @file vfs-providers-local-linux.c++
/// @brief VFS service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::vfs::local
{
    void register_providers();
    void unregister_providers();
}  // namespace platform::vfs::local
