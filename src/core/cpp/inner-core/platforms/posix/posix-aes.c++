/// -*- c++ -*-
//==============================================================================
/// @file posix-aes.c++
/// @brief AES crypto functions on POSIX compatible hosts
/// @author Tor Slettnes
//==============================================================================

#include "posix-aes.h++"

namespace cc::core::platform
{
    PosixAESProvider::PosixAESProvider(const std::string &provider_name)
        : Super(provider_name)
    {
    }
}  // namespace cc::core::platform
