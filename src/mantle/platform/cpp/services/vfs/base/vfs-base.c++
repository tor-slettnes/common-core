// -*- c++ -*-
//==============================================================================
/// @file vfs-provider.c++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-base.h++"

namespace platform::vfs
{
    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProviderInterface> vfs("VFS");
}  // namespace platform::vfs
