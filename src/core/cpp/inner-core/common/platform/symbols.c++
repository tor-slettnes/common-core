/// -*- c++ -*-
//==============================================================================
/// @file symbols.c++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "symbols.h++"

/// Default filesystem paths.
namespace shared::platform
{
    ProviderProxy<SymbolsProvider> symbols("symbols");
}  // namespace shared::platform
