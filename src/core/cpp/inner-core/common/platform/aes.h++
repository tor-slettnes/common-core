/// -*- c++ -*-
//==============================================================================
/// @file aes.h++
/// @brief Symmetric encryption/decryption of text (e.g. saved secrets)
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/bytevector.h++"
#include "provider.h++"

namespace cc::core::platform
{
    class AESProvider : public Provider
    {
        using This = AESProvider;

    protected:
        AESProvider(const std::string &provider_name);

    public:
        void initialize() override;

        void set_key(const std::string &key);
        void set_key(const types::ByteVector &key);
        const types::ByteVector &key() const;

        types::ByteVector encrypt(const std::string &plaintext) const;
        std::string decrypt(const types::ByteVector &encrypted) const;

    private:
        types::ByteVector generate_default_key() const;
        types::Byte salt_byte(const unsigned int index) const;
        types::ByteVector evp_encrypt(const types::ByteVector &input) const;
        types::ByteVector evp_decrypt(const types::ByteVector &input) const;


    private:
        types::ByteVector key_;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<AESProvider> aes;
}  // namespace cc::core::crypto
