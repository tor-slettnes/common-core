/// -*- c++ -*-
//==============================================================================
/// @file aes.c++
/// @brief Symmetric encryption/decryption of text (e.g. saved secrets)
/// @author Tor Slettnes
//==============================================================================

#include "aes.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/err.h>
// #include <openssl/pem.h>
// #include <openssl/hmac.h>

namespace cc::core::platform
{
    constexpr auto AES_KEY_SIZE = 32;

    struct CipherContextDeleter
    {
        void operator()(EVP_CIPHER_CTX *ptr)
        {
            EVP_CIPHER_CTX_free(ptr);
        }
    };

    AESProvider::AESProvider(const std::string &provider_name)
        : Provider(provider_name)
    {
    }

    void AESProvider::initialize()
    {
        Provider::initialize();
        this->set_key(this->generate_default_key());
    }

    types::ByteVector AESProvider::generate_default_key() const
    {
        types::ByteVector key;
        key.reserve(AES_KEY_SIZE);

        for (std::size_t i=0; i<AES_KEY_SIZE; i++)
        {
            types::Byte j = this->salt_byte(AES_KEY_SIZE - i);
            types::Byte v;
            if (i & 0x01)
            {
                j = ~j;
                v = this->salt_byte(j);
            }
            else
            {
                v = ~this->salt_byte(j);
            }

            key.push_back(v);
        }

        return key;
    }

    types::Byte AESProvider::salt_byte(const unsigned int index) const
    {
        static const std::vector<types::ByteVector> salts{
            types::ByteVector::from_hex(SALT_1),
            types::ByteVector::from_hex(SALT_2),
            types::ByteVector::from_hex(SALT_3),
            types::ByteVector::from_hex(SALT_4),
            types::ByteVector::from_hex(SALT_5),
            types::ByteVector::from_hex(SALT_6),
            types::ByteVector::from_hex(SALT_7),
            types::ByteVector::from_hex(SALT_8),
            types::ByteVector::from_hex(SALT_9),
            types::ByteVector::from_hex(SALT_10),
            types::ByteVector::from_hex(SALT_11),
            types::ByteVector::from_hex(SALT_12),
            types::ByteVector::from_hex(SALT_13),
            types::ByteVector::from_hex(SALT_14),
            types::ByteVector::from_hex(SALT_15),
            types::ByteVector::from_hex(SALT_16),
        };
        return salts.at((index >> 4) & 0xF).at(index & 0xF);
    }

    void AESProvider::set_key(const std::string &key)
    {
        this->set_key(types::ByteVector(key));
    }

    void AESProvider::set_key(const types::ByteVector &key)
    {
        if (key.size() > AES_KEY_SIZE)
        {
            this->key_.assign(key.begin(), key.begin() + AES_KEY_SIZE);
        }
        else
        {
            this->key_ = key;
            if (int fill_count = AES_KEY_SIZE - key.size())
            {
                this->key_.insert(this->key_.end(), fill_count, ' ');
            }
        }
    }

    const types::ByteVector &AESProvider::key() const
    {
        return this->key_;
    }

    types::ByteVector AESProvider::encrypt(const std::string &plaintext) const
    {
        return this->evp_encrypt(plaintext);
    }

    std::string AESProvider::decrypt(const types::ByteVector &encrypted) const
    {
        return this->evp_decrypt(encrypted).as_string();
    }

    types::ByteVector AESProvider::evp_encrypt(const types::ByteVector &input) const
    {
        std::unique_ptr<EVP_CIPHER_CTX, CipherContextDeleter> ctx(
            EVP_CIPHER_CTX_new());

        std::string error_preamble = this->name() + "::evp_encrypt(): ";

        if (!ctx)
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to create OpenSSL context");
        }

        core::types::ByteVector output(AES_BLOCK_SIZE + input.size() + AES_BLOCK_SIZE);
        if (!RAND_bytes(output.data(), AES_BLOCK_SIZE))
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to generate random input");
        }

        if (!EVP_EncryptInit_ex(
                ctx.get(),           // ctx
                EVP_aes_256_cbc(),   // cipher
                nullptr,             // impl
                this->key().data(),  // key
                output.data()))      // iv
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to initialize OpenSSL encryption");
        }

        int update_size;
        if (!EVP_EncryptUpdate(
                ctx.get(),                       // ctx
                output.data() + AES_BLOCK_SIZE,  // out
                &update_size,                    // outl
                input.data(),                    // in
                input.size()))                   // inl
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to update OpenSSL encryption");
        }

        int finalize_size;
        if (!EVP_EncryptFinal_ex(
                ctx.get(),                                     // ctx
                output.data() + AES_BLOCK_SIZE + update_size,  // out
                &finalize_size))                               // outl
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to finalize OpenSSL encryption");
        }

        output.resize(AES_BLOCK_SIZE + update_size + finalize_size);
        return output;
    }

    types::ByteVector AESProvider::evp_decrypt(const types::ByteVector &input) const
    {
        std::unique_ptr<EVP_CIPHER_CTX, CipherContextDeleter> ctx(
            EVP_CIPHER_CTX_new());

        std::string error_preamble = this->name() + "::evp_decrypt(): ";

        if (!ctx)
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to create OpenSSL context");
        }

        if (input.size() < AES_BLOCK_SIZE)
        {
            throw core::exception::InvalidArgument(
                error_preamble + "Short input");
        }

        if (!EVP_DecryptInit_ex(
                ctx.get(),           // ctx
                EVP_aes_256_cbc(),   // cipher
                nullptr,             // impl
                this->key().data(),  // key
                input.data()))       // iv
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to initialize OpenSSL decryption");
        }

        ByteVector output(input.size() - AES_BLOCK_SIZE);
        int update_size;

        if (!EVP_DecryptUpdate(
                ctx.get(),                       // ctx
                output.data(),                   // out
                &update_size,                    // outl
                input.data() + AES_BLOCK_SIZE,   // in
                input.size() - AES_BLOCK_SIZE))  // inl
        {
            throw core::exception::RuntimeError(
                error_preamble + "Failed to update OpenSSL decryption operation");
        }

        int finalize_size;

        if (!EVP_DecryptFinal_ex(
                ctx.get(),                    // ctx
                output.data() + update_size,  // outm
                &finalize_size))              // outl
        {
            if (ERR_GET_REASON(ERR_peek_last_error()) == EVP_R_BAD_DECRYPT)
            {
                throw core::exception::InvalidArgument(
                    error_preamble + "Invalid decryption key");
            }
            else
            {
                throw core::exception::RuntimeError(
                    error_preamble + "Encrypted text is corrupted");
            }
        }

        output.resize(update_size + finalize_size);
        return output;
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<AESProvider> aes("aes");
}  // namespace cc::core::platform
