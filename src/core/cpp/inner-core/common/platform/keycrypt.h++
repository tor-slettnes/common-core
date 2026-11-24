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

        void initialize() override;
        virtual void initialize_key();
        virtual types::ByteVector generate_default_key() const = 0;

    public:
        virtual void set_key(
            const types::ByteVector &key) = 0;

        virtual const types::ByteVector &key() const = 0;

        virtual types::ByteVector encrypt(
            const std::string &plaintext) const = 0;

        virtual std::string decrypt(
            const types::ByteVector &encrypted) const = 0;

        std::string encrypt_and_encode(
            const std::string &plaintext) const;

        std::string extract_and_decrypt(
            const std::string &encoded) const;

        std::optional<std::string> try_extract_and_decrypt(
            const std::string &encoded) const;

        bool is_keycrypt_blob(
            const std::string &encoded_text) const;

    private:
        std::pair<std::string, std::string> extract_preamble_and_payload(
            const std::string &encoded_text) const;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<KeyCryptProvider> keycrypt;
}  // namespace cc::core::crypto
