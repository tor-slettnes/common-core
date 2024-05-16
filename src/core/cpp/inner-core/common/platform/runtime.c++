/// -*- c++ -*-
//==============================================================================
/// @file runtime.c++
/// @brief Run-time environment related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "runtime.h++"

namespace core::platform
{
    ProviderProxy<RunTimeProvider> runtime("runtime environment");
}  // namespace core::platform
