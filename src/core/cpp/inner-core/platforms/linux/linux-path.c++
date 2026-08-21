/// -*- c++ -*-
//==============================================================================
/// @file linux-path.c++
/// @brief Path-related functions - Linux specifics
/// @author Tor Slettnes
//==============================================================================

#include "linux-path.h++"

namespace cc::core::platform
{
    LinuxPathProvider::LinuxPathProvider(const fs::path &exec_path)
        : Super("LinuxPathProvider", exec_path),
          proc_exec_path_(this->readlink("/proc/self/exe"))
    {
    }

    fs::path LinuxPathProvider::exec_path() const noexcept
    {
        return !this->proc_exec_path_.empty()
                 ? this->proc_exec_path_
                 : Super::exec_path();
    }
}  // namespace cc::core::platform
