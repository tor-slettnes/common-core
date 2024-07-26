/// -*- c++ -*-
//==============================================================================
/// @file protobuf-system-types.c++
/// @brief Encode/decode routines for types in `system.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-system-types.h++"
#include "protobuf-standard-types.h++"

namespace protobuf
{
    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                cc::system::TimeZoneInfo *encoded) noexcept
    {
        encoded->set_shortname(native.shortname);
        encode(native.offset, encoded->mutable_offset());
        encode(native.stdoffset, encoded->mutable_offset());
        encoded->set_dst(native.dst);
    }

    void decode(const cc::system::TimeZoneInfo encoded,
                core::dt::TimeZoneInfo *native) noexcept
    {
        native->shortname = encoded.shortname();
        decode(encoded.offset(), &native->offset);
        decode(encoded.stdoffset(), &native->stdoffset);
        native->dst = encoded.dst();
    }

}  // namespace protobuf
