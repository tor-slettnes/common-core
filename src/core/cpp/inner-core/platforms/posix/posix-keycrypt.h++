/// -*- c++ -*-
//==============================================================================
/// @file posix-keycrypt.h++
/// @brief KeyCrypt crypto functions on POSIX compatible hosts
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/keycrypt.h++"

namespace cc::core::platform
{
    /// @brief POSIX provider for KeyCrypt crypto functions
    class PosixKeyCryptProvider : public KeyCryptProvider
    {
        using This = PosixKeyCryptProvider;
        using Super = KeyCryptProvider;

    protected:
        PosixKeyCryptProvider(const std::string &provider_name = "PosixKeyCryptProvider");

    public:
        void initialize() override;

        void set_key(const types::ByteVector &key) override;
        const types::ByteVector &key() const override;

        types::ByteVector encrypt(const std::string &plaintext) const override;
        std::string decrypt(const types::ByteVector &encrypted) const override;

    private:
        types::ByteVector generate_default_key() const;
        types::Byte salt_byte(const unsigned int index) const;
        types::ByteVector evp_encrypt(const types::ByteVector &input) const;
        types::ByteVector evp_decrypt(const types::ByteVector &input) const;

    private:
        types::ByteVector key_;
    };

}  // namespace cc::core::platform
