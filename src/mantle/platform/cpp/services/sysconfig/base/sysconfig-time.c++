// -*- c++ -*-
//==============================================================================
/// @file sysconfig-time.c++
/// @brief SysConfig service - Abstract C++ interface
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-time.h++"
#include "types/symbolmap.h++"

namespace sysconfig
{
    //==========================================================================
    // TimeSync

    core::types::SymbolMap<TimeSync> tsync_symbols = {
        {TSYNC_NONE, "TSYNC_NONE"},
        {TSYNC_NTP, "TSYNC_NTP"},
    };

    std::ostream &operator<<(std::ostream &stream, TimeSync sync)
    {
        return tsync_symbols.to_stream(stream, sync, std::to_string(sync));
    }

    std::istream &operator>>(std::istream &stream, TimeSync &sync)
    {
        return tsync_symbols.from_stream(stream, &sync, TSYNC_NONE);
    }

    //==========================================================================
    // TimeConfig

    std::ostream &operator<<(std::ostream &stream, const TimeConfig &tc)
    {
        stream << "{synchronization=" << tc.synchronization;
        if (!tc.servers.empty())
        {
            stream << ", servers=" << tc.servers;
        }
        stream << "}";
        return stream;
    }

    //==========================================================================
    // Time provider proxy

    core::platform::ProviderProxy<TimeConfigInterface> time("time");

    //==========================================================================
    // Signals

    core::signal::DataSignal<core::dt::TimePoint> signal_time("TimePoint");
    core::signal::DataSignal<TimeConfig> signal_timeconfig("TimeConfig", true);
}  // namespace sysconfig
