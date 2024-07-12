/// -*- c++ -*-
//==============================================================================
/// @file posix-host.c++
/// @brief Host related functions on POSIX compatible hosts
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-host.h++"
#include "platform/process.h++"
#include "status/exceptions.h++"

#include <sys/utsname.h>
#include <unistd.h>

#define HOST_NAME_MAX 64

namespace core::platform
{
    std::string PosixHostProvider::get_host_name() const
    {
        char hostname[HOST_NAME_MAX + 1];
        hostname[HOST_NAME_MAX] = '\0';

        if (gethostname(hostname, HOST_NAME_MAX) == -1)
        {
            throw exception::SystemError();
        }
        return hostname;
    }

    void PosixHostProvider::set_host_name(const std::string &hostname)
    {
        if (::sethostname(hostname.data(), hostname.size()) != 0)
        {
            throwf(exception::SystemError,
                   "Failed to set hostname %r",
                   hostname);
        }
    }

    std::string PosixHostProvider::get_system_name() const
    {
        struct utsname un;
        uname(&un);
        return un.sysname;
    }

    std::string PosixHostProvider::get_system_version() const
    {
        struct utsname un;
        uname(&un);
        return un.release;
    }

    void PosixHostProvider::reboot()
    {
        core::platform::process->invoke_check({"/sbin/reboot"});
    }
}  // namespace core::platform
