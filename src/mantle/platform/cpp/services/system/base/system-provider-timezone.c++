// -*- c++ -*-
//==============================================================================
/// @file system-timezone-api.c++
/// @brief System service - Time Zone configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-provider-timezone.h++"

namespace platform::system
{
    // TimeZoneSpec TimeZone::get_timezone_spec() const
    // {
    //     return this->get_timezone_spec(this->get_configured_timezone().zonename());
    // }

    core::platform::ProviderProxy<TimeZone> timezone("timezone");

    core::signal::DataSignal<core::dt::TimeZoneInfo> signal_tzinfo("TimeZoneInfo", true);
    core::signal::DataSignal<TimeZoneConfig> signal_tzconfig("TimeZoneConfig", true);
}  // namespace platform::system
