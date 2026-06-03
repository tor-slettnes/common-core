/// -*- c++ -*-
//==============================================================================
/// @file user.c++
/// @brief User-related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "user.h++"

namespace cc::core::platform
{
    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<UserProvider> user("user");
}  // namespace cc::core::platform
