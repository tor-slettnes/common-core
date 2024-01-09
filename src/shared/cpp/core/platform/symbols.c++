/// -*- c++ -*-
//==============================================================================
/// @file symbols.c++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "symbols.h++"

/// Default filesystem paths.
namespace cc::platform
{
    ProviderProxy<SymbolsProvider> symbols("symbols");
}  // namespace cc::platform
