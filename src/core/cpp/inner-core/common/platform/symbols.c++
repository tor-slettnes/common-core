/// -*- c++ -*-
//==============================================================================
/// @file symbols.c++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "symbols.h++"
#include "string/format.h++"

/// Default filesystem paths.
namespace core::platform
{
    std::string SymbolsProvider::errno_name(int num) const noexcept
    {
        return "";
    }

    ProviderProxy<SymbolsProvider> symbols("symbols");


}  // namespace core::platform
