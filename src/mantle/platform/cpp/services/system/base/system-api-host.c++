// -*- c++ -*-
//==============================================================================
/// @file system-api-host.c++
/// @brief System service - Host Configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-api-host.h++"

namespace platform::system
{
    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<HostConfigProvider> hostconfig("hostconfig");

    //==========================================================================
    // Signals

    core::signal::DataSignal<HostInfo> signal_hostinfo("hostinfo", true);
}  // namespace platform::system
