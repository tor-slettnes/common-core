/// -*- c++ -*-
//==============================================================================
/// @file protobuf-system-types.h++
/// @brief Encode/decode routines for types in `system.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system.pb.h"  // generated from `system.proto`
#include "chrono/date-time.h++"

namespace core::protobuf
{
    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                cc::platform::system::TimeZoneInfo *encoded) noexcept;

    void decode(const cc::platform::system::TimeZoneInfo encoded,
                core::dt::TimeZoneInfo *native) noexcept;

}  // namespace core::protobuf
