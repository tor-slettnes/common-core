/// -*- c++ -*-
//==============================================================================
/// @file linux-time.c++
/// @brief Time related functions on Linux
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "linux-time.h++"
#include "platform/process.h++"
#include "chrono/date-time.h++"

namespace core::platform
{
    constexpr auto TIMEDATECTL_PATH = "/usr/bin/timedatectl";
    constexpr auto TIMEDATECTL_SHOW = "show";
    constexpr auto TIMEDATECTL_SETNTP = "set-ntp";
    constexpr auto TIMEDATECTL_SETTIME = "set-time";
    constexpr auto TIMEDATECTL_LOCAL = true;
    constexpr auto SETTING_NTP = "NTP";
    constexpr auto SETTING_NTP_ON = "yes";
    constexpr auto SETTING_NTP_OFF = "no";

    LinuxTimeProvider::LinuxTimeProvider()
        : Super("LinuxTimeProvider")
    {
    }

    void LinuxTimeProvider::set_time(const dt::TimePoint &tp)
    {
        process->invoke_check({
            TIMEDATECTL_PATH,
            TIMEDATECTL_SETTIME,
            dt::to_string(tp, TIMEDATECTL_LOCAL, 0, "%F %T"),
        });
    }

}  // namespace core::platform
