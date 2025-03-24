// -*- c++ -*-
//==============================================================================
/// @file sysconfig-systemd-time.c++
/// @brief SysConfig implementation via SystemD - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-systemd-time.h++"
#include "platform/symbols.h++"
#include "platform/process.h++"
#include "chrono/date-time.h++"
#include "types/filesystem.h++"
#include "status/exceptions.h++"

namespace sysconfig::native
{
    constexpr auto TIMEDATECTL_PATH = "/usr/bin/timedatectl";
    constexpr auto TIMEDATECTL_SHOW = "show";
    constexpr auto TIMEDATECTL_SETNTP = "set-ntp";
    constexpr auto TIMEDATECTL_SETTIME = "set-time";
    constexpr auto SETTING_NTP = "NTP";
    constexpr auto SETTING_NTP_ON = "yes";
    constexpr auto SETTING_NTP_OFF = "no";

    SystemdTimeConfigProvider::SystemdTimeConfigProvider()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    bool SystemdTimeConfigProvider::is_pertinent() const
    {
        return fs::exists(TIMEDATECTL_PATH);
    }

    void SystemdTimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        core::platform::process->invoke_check({
            TIMEDATECTL_PATH,
            TIMEDATECTL_SETTIME,
            core::dt::to_string(tp, false, 0, "%F %T UTC"),
        });
    }

    void SystemdTimeConfigProvider::set_ntp(bool ntp)
    {
        core::platform::process->invoke_check({
            TIMEDATECTL_PATH,
            TIMEDATECTL_SETNTP,
            ntp ? SETTING_NTP_ON : SETTING_NTP_OFF,
        });
    }

    bool SystemdTimeConfigProvider::get_ntp() const
    {
        std::string setting = this->read_settings().get(SETTING_NTP);
        return core::str::convert_to<bool>(setting, false);
    }

    std::vector<std::string> SystemdTimeConfigProvider::get_ntp_servers() const
    {
        return {};
    }

    core::types::ValueMap<std::string, std::string> SystemdTimeConfigProvider::read_settings() const
    {
        core::platform::InvocationResult result = core::platform::process->invoke_capture(
            {TIMEDATECTL_PATH, TIMEDATECTL_SHOW});

        if (!result.status->success())
        {
            logf_notice(
                "Command %r returned status %d (%s): %s"
                "\n\tstdout=%r"
                "\n\tstderr=%r",
                TIMEDATECTL_PATH,
                result.status->combined_code(),
                result.status->symbol(),
                result.status->text(),
                result.stdout->str(),
                result.stderr->str());

            throw core::exception::InvocationError(
                TIMEDATECTL_PATH,
                result.status);
        }

        core::types::ValueMap<std::string, std::string> valuemap;
        for (const std::string &line : core::str::splitlines(result.stdout->str()))
        {
            std::vector<std::string> subparts = core::str::split(line, "=", 1);
            if (subparts.size() == 2)
            {
                valuemap.insert_or_assign(subparts.front(), subparts.back());
            }
        }
        return valuemap;
    }

}  // namespace sysconfig::systemd
