/// -*- c++ -*-
//==============================================================================
/// @file qnx-symbols.c++
/// @brief Functions to produce symbols - QNX verison
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "qnx-symbols.h++"

namespace cc::platform
{
    QNXSymbolsProvider::QNXSymbolsProvider(const std::string &name)
        : PosixSymbolsProvdier(name)
    {
    }

    std::string QNXSymbolsProvider::uuid() const noexcept
    {
        uuid_t uuid;
        uuid_generate(uuid);

        char buffer[UUID_STR_LEN];
        uuid_unparse(uuid, buffer);
        return buffer;
    }
};

}  // namespace cc::platform
