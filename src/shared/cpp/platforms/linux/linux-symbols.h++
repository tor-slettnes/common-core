/// -*- c++ -*-
//==============================================================================
/// @file linux-symbols.h++
/// @brief Functions to produce symbols - Linux verison
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-symbols.h++"

namespace cc::platform
{
    class LinuxSymbolsProvider : public PosixSymbolsProvider
    {
        using This = LinuxSymbolsProvider;
        using Super = PosixSymbolsProvider;

    public:
        LinuxSymbolsProvider(const std::string &name = "LinuxSymbolsProvider");

        std::string uuid() const noexcept override;
    };

}  // namespace cc::platform
