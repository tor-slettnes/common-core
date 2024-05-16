/// -*- c++ -*-
//==============================================================================
/// @file timezone.c++
/// @brief Timezone functions - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "timezone.h++"

namespace core::platform
{
    ProviderProxy<TimeZoneProvider> timezone("timezone");

}  // namespace core::platform
