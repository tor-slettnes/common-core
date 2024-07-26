// -*- c++ -*-
//==============================================================================
/// @file vfs-provider.c++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-provider.h++"

namespace platform::vfs
{
    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<Provider> provider("VFS");
}
