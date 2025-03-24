// -*- c++ -*-
//==============================================================================
/// @file netconfig-signals.c++
/// @brief NetConfig service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-signals.h++"
#include "thread/signaltemplate.h++"

namespace netconfig
{
    core::signal::DataSignal<GlobalData::ptr> signal_globaldata(
        "GlobalData",
        true);

    core::signal::MappingSignal<AccessPointData::ptr> signal_accesspoint(
        "AccessPoint",
        true);

    core::signal::MappingSignal<ConnectionData::ptr> signal_connection(
        "Connection",
        true);

    core::signal::MappingSignal<ActiveConnectionData::ptr> signal_active_connection(
        "ActiveConnection",
        true);

    core::signal::MappingSignal<DeviceData::ptr> signal_device(
        "Device",
        true);
}  // namespace netconfig
