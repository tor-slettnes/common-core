/// -*- c++ -*-
//==============================================================================
/// @file keycrypt.h++
/// @brief Symmetric encryption/decryption of text (e.g. saved secrets)
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/bytevector.h++"
#include "provider.h++"

namespace cc::core::platform
{
    class KeyCryptProvider : public Provider
    {
        using This = KeyCryptProvider;

    protected:
        KeyCryptProvider(const std::string &provider_name);

    public:
        virtual void set_key(const types::ByteVector &key) = 0;
        virtual const types::ByteVector &key() const = 0;

        virtual types::ByteVector encrypt(const std::string &plaintext) const = 0;
        virtual std::string decrypt(const types::ByteVector &encrypted) const = 0;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<KeyCryptProvider> keycrypt;
}  // namespace cc::core::crypto
