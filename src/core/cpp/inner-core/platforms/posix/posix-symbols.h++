/// -*- c++ -*-
//==============================================================================
/// @file posix-symbols.h++
/// @brief Internal symbols - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/symbols.h++"

namespace cc::platform
{
    class PosixSymbolsProvider : public SymbolsProvider
    {
        using This = PosixSymbolsProvider;
        using Super = SymbolsProvider;

    public:
        PosixSymbolsProvider(const std::string &name = "PosixSymbolsProvider");

        std::string cpp_demangle(
            const std::string &abiname,
            bool stem_only) const noexcept override;
    };

}  // namespace cc::platform
