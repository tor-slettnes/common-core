// -*- c++ -*-
//==============================================================================
/// @file vfs-providers-local.h++
/// @brief VFS service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace vfs::local
{
    void register_providers();
    void unregister_providers();
}  // namespace vfs::local
