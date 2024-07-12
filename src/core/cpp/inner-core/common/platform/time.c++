/// -*- c++ -*-
//==============================================================================
/// @file time.c++
/// @brief Date/Time functions - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "time.h++"

namespace core::platform
{
    ProviderProxy<TimeProvider> time("time");
}  // namespace core::platform
