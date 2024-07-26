/// -*- c++ -*-
//==============================================================================
/// @file user.h++
/// @brief User-related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"

namespace core::platform
{
    using UID = std::uint32_t;
    using GID = std::uint32_t;

    /// @brief Abstract provider for user-related functions
    class UserProvider : public Provider
    {
        using Super = Provider;

    protected:
        using Super::Super;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<UserProvider> user;
}  // namespace core::platform
