// -*- c++ -*-
//==============================================================================
/// @file udev-signals.c++
/// @brief Monitor UDev events - Signals
/// @author Tor Slettnes
//==============================================================================

#include "udev-signals.h++"

namespace cc::core::udev
{
    core::signal::SharedMappingSignal<DeviceInfo> signal_device(
        "device",
        true);
}  // namespace cc::core::udev
