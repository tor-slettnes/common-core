/// -*- c++ -*-
//==============================================================================
/// @file symbols.c++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "symbols.h++"
#include "string/format.h++"

#include <string.h>

/// Default filesystem paths.
namespace core::platform
{
    std::string SymbolsProvider::errno_name(int num) const noexcept
    {
        return "";
    }

    std::string SymbolsProvider::errno_string(int num) const noexcept
    {
        return ::strerror(num);
    }

    ProviderProxy<SymbolsProvider> symbols("symbols");

}  // namespace core::platform
