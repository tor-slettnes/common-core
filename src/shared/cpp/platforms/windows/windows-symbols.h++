/// -*- c++ -*-
//==============================================================================
/// @file windows-symbols.h++
/// @brief Internal symbols - Windows specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/symbols.h++"

namespace shared::platform
{
    class WindowsSymbolsProvider : public SymbolsProvider
    {
        using This = WindowsSymbolsProvider;
        using Super = SymbolsProvider;

    public:
        WindowsSymbolsProvider();

        std::string cpp_demangle(
            const std::string &abiname,
            bool stem_only) const noexcept override;
    };

}  // namespace shared::platform
