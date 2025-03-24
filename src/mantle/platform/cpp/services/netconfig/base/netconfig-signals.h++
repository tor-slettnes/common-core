// -*- c++ -*-
//==============================================================================
/// @file netconfig-signals.h++
/// @brief NetConfig service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "netconfig-types.h++"
#include "thread/signaltemplate.h++"

namespace netconfig
{
    extern core::signal::DataSignal<GlobalData::ptr> signal_globaldata;
    extern core::signal::MappingSignal<ConnectionData::ptr> signal_connection;
    extern core::signal::MappingSignal<ActiveConnectionData::ptr> signal_active_connection;
    extern core::signal::MappingSignal<AccessPointData::ptr> signal_accesspoint;
    extern core::signal::MappingSignal<DeviceData::ptr> signal_device;
}  // namespace netconfig
