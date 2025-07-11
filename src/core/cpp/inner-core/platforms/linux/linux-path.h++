/// -*- c++ -*-
//==============================================================================
/// @file linux-path.h++
/// @brief Path-related functions - Linux specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-path.h++"

namespace core::platform
{
    class LinuxPathProvider : public PosixPathProvider
    {
        using This = LinuxPathProvider;
        using Super = PosixPathProvider;

    protected:
        LinuxPathProvider(const std::string &exec_name);

    public:
        fs::path exec_path() const noexcept override;

    private:
        fs::path exec_path_;
    };

}  // namespace core::platform
