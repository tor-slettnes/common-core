// -*- c++ -*-
//==============================================================================
/// @file system-provider-time.c++
/// @brief System service - Abstract C++ interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-provider-time.h++"

namespace platform::system
{
    core::platform::ProviderProxy<TimeProvider> time("time");

    core::signal::DataSignal<core::dt::TimePoint> signal_time("TimePoint");
    core::signal::DataSignal<TimeConfig> signal_timeconfig("TimeConfig", true);
}  // namespace platform::system