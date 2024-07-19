// -*- c++ -*-
//==============================================================================
/// @file network-signals.h++
/// @brief Network service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network-types.h++"
#include "thread/signaltemplate.h++"

namespace platform::network
{
    extern core::signal::DataSignal<GlobalDataRef> signal_globaldata;
    extern core::signal::MappingSignal<AccessPointRef> signal_accesspoint;
    extern core::signal::MappingSignal<ConnectionRef> signal_connection;
    extern core::signal::MappingSignal<ActiveConnectionRef> signal_active_connection;
    extern core::signal::MappingSignal<DeviceRef> signal_device;
}  // namespace platform::network
