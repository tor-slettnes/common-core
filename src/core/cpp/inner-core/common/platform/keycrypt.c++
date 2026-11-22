/// -*- c++ -*-
//==============================================================================
/// @file keycrypt.c++
/// @brief Symmetric encryption/decryption of text (e.g. saved secrets)
/// @author Tor Slettnes
//==============================================================================

#include "keycrypt.h++"

namespace cc::core::platform
{
    KeyCryptProvider::KeyCryptProvider(const std::string &provider_name)
        : Provider(provider_name)
    {
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<KeyCryptProvider> keycrypt("keycrypt");
}  // namespace cc::core::platform
