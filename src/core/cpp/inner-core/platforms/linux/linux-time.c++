/// -*- c++ -*-
//==============================================================================
/// @file linux-time.c++
/// @brief Time related functions on Linux
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "linux-time.h++"
#include "platform/process.h++"
#include "platform/symbols.h++"
#include "chrono/date-time.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::platform
{
    constexpr auto TIMEDATECTL_PATH = "/usr/bin/timedatectl";
    constexpr auto TIMEDATECTL_SHOW = "show";
    constexpr auto TIMEDATECTL_SETNTP = "set-ntp";
    constexpr auto TIMEDATECTL_SETTIME = "set-time";
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
            dt::to_string(tp, false, 0, "%F %T UTC"),
        });
    }

    void LinuxTimeProvider::set_ntp(bool ntp)
    {
        process->invoke_check({
            TIMEDATECTL_PATH,
            TIMEDATECTL_SETNTP,
            ntp ? SETTING_NTP_ON : SETTING_NTP_OFF,
        });
    }

    bool LinuxTimeProvider::get_ntp() const
    {
        std::string setting = this->read_settings().get(SETTING_NTP);
        return core::str::convert_to<bool>(setting, false);
    }

    void LinuxTimeProvider::set_ntp_servers(const std::vector<std::string> &servers)
    {
        throw std::invalid_argument("set_ntp_servers() is not implemented on this platform");
    }

    std::vector<std::string> LinuxTimeProvider::get_ntp_servers() const
    {
        return {};
    }

    core::types::ValueMap<std::string, std::string> LinuxTimeProvider::read_settings() const
    {
        std::stringstream out, err;
        try
        {
            process->invoke_check(
                {TIMEDATECTL_PATH, TIMEDATECTL_SHOW},
                {},       // cwd
                nullptr,  // stdin
                &out,     // stdout
                &err);    // stderr
        }
        catch (const std::system_error &e)
        {
            logf_notice(
                "Command %r returned status %d (%s): %s"
                "\n\tstdout=%r"
                "\n\tstderr=%r",
                TIMEDATECTL_PATH,
                e.code().value(),
                symbols->errno_name(e.code().value()),
                e,
                out.str(),
                err.str());
        }

        core::types::ValueMap<std::string, std::string> valuemap;
        for (const std::string &line : str::splitlines(out.str()))
        {
            std::vector<std::string> subparts = str::split(line, "=", 1);
            if (subparts.size() == 2)
            {
                valuemap.insert_or_assign(subparts.front(), subparts.back());
            }
        }
        return valuemap;
    }

}  // namespace core::platform
