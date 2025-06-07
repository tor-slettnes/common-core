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
        : Super("LinuxPathProvider", exec_name),
          exec_path_(this->readlink("/proc/self/exe"))
    {
    }

    fs::path LinuxPathProvider::exec_path() const noexcept
    {
        return !this->exec_path_.empty() ? this->exec_path_
                                         : Super::exec_path();
    }
}  // namespace core::platform
