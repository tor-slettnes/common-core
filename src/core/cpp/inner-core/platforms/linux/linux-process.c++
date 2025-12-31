// -*- c++ -*-
//==============================================================================
/// @file linux-process.c++
/// @brief Process invocation - Linux implementations
/// @author Tor Slettnes
//==============================================================================

#include "linux-process.h++"
#include "platform/path.h++"
#include "string/format.h++"

namespace core::platform
{
    LinuxProcessProvider::LinuxProcessProvider(const std::string &name)
        : PosixProcessProvider(name)
    {
    }

    std::string LinuxProcessProvider::get_process_name_by_pid(PID pid) const
    {
        return path->readtext(
            core::str::format("/proc/%s/comm", pid));
    }

}  // namespace core::platform
