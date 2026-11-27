// -*- c++ -*-
//==============================================================================
/// @file udev-signals.h++
/// @brief Monitor UDev events - Signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "udev-deviceinfo.h++"
#include "signal/signals.h++"

namespace cc::core::udev
{
    extern core::signal::SharedMappingSignal<DeviceInfo> signal_device;
}  // namespace cc::core::udev
