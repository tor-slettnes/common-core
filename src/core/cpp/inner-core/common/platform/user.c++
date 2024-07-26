/// -*- c++ -*-
//==============================================================================
/// @file user.c++
/// @brief User-related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "user.h++"

namespace core::platform
{
    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<UserProvider> user("user");
}  // namespace core::platform
