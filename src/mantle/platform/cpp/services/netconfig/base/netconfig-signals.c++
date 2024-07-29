// -*- c++ -*-
//==============================================================================
/// @file netconfig-signals.c++
/// @brief NetConfig service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-signals.h++"
#include "thread/signaltemplate.h++"

namespace platform::netconfig
{
    core::signal::DataSignal<GlobalDataRef> signal_globaldata(
        "GlobalData",
        true);

    core::signal::MappingSignal<AccessPointRef> signal_accesspoint(
        "AccessPoint",
        true);

    core::signal::MappingSignal<ConnectionRef> signal_connection(
        "Connection",
        true);

    core::signal::MappingSignal<ActiveConnectionRef> signal_active_connection(
        "ActiveConnection",
        true);

    core::signal::MappingSignal<DeviceRef> signal_device(
        "Device",
        true);
}  // namespace platform::netconfig
