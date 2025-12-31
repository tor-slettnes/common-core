/// -*- c++ -*-
//==============================================================================
/// @file linux-host.c++
/// @brief Host related functions on Linux
/// @author Tor Slettnes
//==============================================================================

#include "linux-host.h++"
#include "platform/path.h++"
#include "platform/process.h++"
#include "string/misc.h++"

#include <fstream>

namespace core::platform
{
    constexpr auto HOSTNAME_FILE = "/etc/hostname";
    constexpr auto OS_RELEASE_FILE = "/etc/os-release";

    LinuxHostProvider::LinuxHostProvider()
        : Super("LinuxHostProvider"),
          os_release(This::read_os_release())
    {
    }

    void LinuxHostProvider::set_host_name(const std::string &hostname)
    {
        Super::set_host_name(hostname);
        std::ofstream hostnamefile(HOSTNAME_FILE);
        hostnamefile.exceptions(std::ofstream::failbit);
        hostnamefile << hostname << std::endl;
    }

    std::string LinuxHostProvider::get_system_release_flavor_short() const
    {
        return this->os_release.get("ID");
    }

    std::string LinuxHostProvider::get_system_release_flavor_long() const
    {
        return this->os_release.get("NAME");
    }

    std::string LinuxHostProvider::get_system_release_version_id() const
    {
        return this->os_release.get("VERSION_ID");
    }

    std::string LinuxHostProvider::get_system_release_version_name() const
    {
        return this->os_release.get("VERSION_CODENAME");
    }

    void LinuxHostProvider::reboot()
    {
        core::platform::process->invoke_check({"/bin/systemctl", "reboot"});
    }

    types::ValueMap<std::string, std::string> LinuxHostProvider::read_os_release()
    {
        types::ValueMap<std::string, std::string> kvmap;
        for (const std::string line : str::splitlines(path->readtext(OS_RELEASE_FILE)))
        {
            std::vector<std::string> parts = str::split(line, "=", 1, true);
            if (parts.size() == 2)
            {
                kvmap.insert_or_assign(parts.front(), str::unquoted(parts.back()));
            }
        }
        return kvmap;
    }

}  // namespace core::platform
