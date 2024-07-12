/// -*- c++ -*-
//==============================================================================
/// @file posix-time.h++
/// @brief Time related functions on POSIX compatible systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/time.h++"

namespace core::platform
{
    class PosixTimeProvider : public TimeProvider
    {
        using This = PosixTimeProvider;
        using Super = TimeProvider;

    protected:
        using Super::Super;
    };

}  // namespace core::platform
