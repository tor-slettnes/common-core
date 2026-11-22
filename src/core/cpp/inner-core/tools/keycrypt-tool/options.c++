// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Secrets tool - command line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "platform/keycrypt.h++"

namespace cc::core::platform
{
    Options::Options()
        : Super()
    {
        this->describe("Key encryption/decryption tool.");
    }

    void Options::add_options()
    {
        Super::add_options();
        this->add_commands();
    }

    void Options::add_commands()
    {
        this->add_command(
            "showkey",
            {},
            "Show encryption key.",
            std::bind(&Options::showkey, this));

        this->add_command(
            "encrypt",
            {"SECRET"},
            "Encrypt SECRET and encode with base64.",
            std::bind(&Options::encrypt, this));

        this->add_command(
            "decrypt",
            {"CIPHERTEXT"},
            "Decrypt base64-encoded secret",
            std::bind(&Options::decrypt, this));
    }

    void Options::showkey()
    {
        std::cout << platform::keycrypt->key().to_hex()
                  << std::endl;
    }

    void Options::encrypt()
    {
        std::string secret = this->get_arg("SECRET");

        std::cout << platform::keycrypt->encrypt(secret).to_base64()
                  << std::endl;
    }

    void Options::decrypt()
    {
        std::string encoded = this->get_arg("CIPHERTEXT");

        std::cout << platform::keycrypt->decrypt(types::ByteVector::from_base64(encoded))
                  << std::endl;
    }
}  // namespace cc::core::platform
