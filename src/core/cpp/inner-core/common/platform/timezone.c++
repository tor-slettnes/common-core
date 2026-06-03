/// -*- c++ -*-
//==============================================================================
/// @file timezone.c++
/// @brief Timezone functions - abstract interface
/// @author Tor Slettnes
//==============================================================================

#include "timezone.h++"

namespace cc::core::platform
{
    ProviderProxy<TimeZoneProvider> timezone("timezone");

}  // namespace cc::core::platform
