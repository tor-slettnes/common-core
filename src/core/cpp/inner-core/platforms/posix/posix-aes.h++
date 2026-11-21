/// -*- c++ -*-
//==============================================================================
/// @file posix-aes.h++
/// @brief AES crypto functions on POSIX compatible hosts
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/aes.h++"

namespace cc::core::platform
{
    /// @brief POSIX provider for AES crypto functions
    class PosixAESProvider : public AESProvider
    {
        using This = PosixAESProvider;
        using Super = AESProvider;

    protected:
        PosixAESProvider(const std::string &provider_name = "PosixAESProvider");
    };

}  // namespace cc::core::platform
