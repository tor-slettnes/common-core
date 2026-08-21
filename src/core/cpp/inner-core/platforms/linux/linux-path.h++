/// -*- c++ -*-
//==============================================================================
/// @file linux-path.h++
/// @brief Path-related functions - Linux specifics
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "posix-path.h++"

namespace cc::core::platform
{
    class LinuxPathProvider : public PosixPathProvider
    {
        using This = LinuxPathProvider;
        using Super = PosixPathProvider;

    protected:
        LinuxPathProvider(const fs::path &exec_path);

    public:
        fs::path exec_path() const noexcept override;

    private:
        fs::path proc_exec_path_;
    };

}  // namespace cc::core::platform
