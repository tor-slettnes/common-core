/// -*- c++ -*-
//==============================================================================
/// @file keycrypt.c++
/// @brief Symmetric encryption/decryption of text (e.g. saved secrets)
/// @author Tor Slettnes
//==============================================================================

#include "keycrypt.h++"
#include "status/exceptions.h++"
#include "runtime.h++"

namespace cc::core::platform
{
    const std::string ENV_MASTER_KEY = "KEYCRYPT_MASTER_KEY";

    const std::string ENCODE_PREAMBLE{"KEYCRYPT"};
    const std::string ENCODE_PREFIX{ENCODE_PREAMBLE + ".v1"};

    KeyCryptProvider::KeyCryptProvider(const std::string &provider_name)
        : Provider(provider_name)
    {
    }

    void KeyCryptProvider::initialize()
    {
        Provider::initialize();
        this->initialize_key();
    }

    void KeyCryptProvider::initialize_key()
    {
        if (auto master_key = runtime->getenv(ENV_MASTER_KEY))
        {
            this->set_key(master_key.value());
        }
        else
        {
            this->set_key(this->generate_default_key());
        }
    }

    std::string KeyCryptProvider::encrypt_and_encode(
        const std::string &plaintext) const
    {
        return ENCODE_PREFIX + ";" + this->encrypt(plaintext).to_base64();
    }

    std::string KeyCryptProvider::extract_and_decrypt(
        const std::string &encoded) const
    {
        auto [preamble, payload] = this->extract_preamble_and_payload(encoded);
        if (preamble.empty())
        {
            throwf(
                exception::InvalidArgument,
                "Not a valid KeyCrypt string");
        }
        else if (preamble != ENCODE_PREFIX)
        {
            throwf(
                exception::InvalidArgument,
                "Unsupported KeyCrypt preamble: %s (expected: %s)",
                preamble,
                ENCODE_PREFIX);
        }

        return this->decrypt(types::ByteVector::from_base64(payload));
    }

    std::optional<std::string> KeyCryptProvider::try_extract_and_decrypt(
        const std::string &encoded) const
    {
        try
        {
            return this->extract_and_decrypt(encoded);
        }
        catch (...)
        {
            return {};
        }
    }

    bool KeyCryptProvider::is_keycrypt_blob(const std::string &encoded_text) const
    {
        auto [preamble, payload] = this->extract_preamble_and_payload(encoded_text);
        return !preamble.empty();
    }

    std::pair<std::string, std::string>
    KeyCryptProvider::extract_preamble_and_payload(
        const std::string &encoded_text) const
    {
        const std::regex rx_preamble_and_payload{
            "^(?:\\s*(" + ENCODE_PREAMBLE +
            "\\.v\\d+)[\\W]?)?"  // (1) preamble
            "\\s*([^=]+=*)$"     // (2) payload
        };

        std::smatch match;
        std::pair<std::string, std::string> value_pair;

        if (std::regex_search(encoded_text, match, rx_preamble_and_payload))
        {
            return {match.str(1), match.str(2)};
        }

        return {};
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<KeyCryptProvider> keycrypt("keycrypt");
}  // namespace cc::core::platform
