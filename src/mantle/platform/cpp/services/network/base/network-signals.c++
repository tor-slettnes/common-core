// -*- c++ -*-
//==============================================================================
/// @file network-signals.c++
/// @brief Network service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-signals.h++"
#include "thread/signaltemplate.h++"

namespace platform::network
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
}  // namespace platform::network
