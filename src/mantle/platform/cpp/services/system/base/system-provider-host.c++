// -*- c++ -*-
//==============================================================================
/// @file system-provider-host.c++
/// @brief System service - Host Configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-provider-host.h++"

namespace platform::system
{
    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<HostConfigProvider> hostconfig("hostconfig");

    //==========================================================================
    // Signals

    core::signal::DataSignal<HostInfo> signal_hostinfo("hostinfo", true);
}  // namespace platform::system
