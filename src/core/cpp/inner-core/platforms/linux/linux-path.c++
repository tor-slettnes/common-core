/// -*- c++ -*-
//==============================================================================
/// @file linux-path.c++
/// @brief Path-related functions - Linux specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "linux-path.h++"

namespace core::platform
{
    LinuxPathProvider::LinuxPathProvider(const std::string &exec_name)
        : Super("LinuxPathProvider", exec_name)
    {
    }

    fs::path LinuxPathProvider::exec_path() const noexcept
    {
        static fs::path path = this->readlink("/proc/self/exe");
        return path.empty() ? Super::exec_path() : path;
    }
}  // namespace core::platform
