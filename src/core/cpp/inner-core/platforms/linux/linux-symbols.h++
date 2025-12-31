/// -*- c++ -*-
//==============================================================================
/// @file linux-symbols.h++
/// @brief Functions to produce symbols - Linux verison
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "posix-symbols.h++"

namespace core::platform
{
    class LinuxSymbolsProvider : public PosixSymbolsProvider
    {
        using This = LinuxSymbolsProvider;
        using Super = PosixSymbolsProvider;

    public:
        LinuxSymbolsProvider(const std::string &name = "LinuxSymbolsProvider");

        std::string uuid() const noexcept override;
        std::string errno_name(int num) const noexcept override;
    };

}  // namespace core::platform
