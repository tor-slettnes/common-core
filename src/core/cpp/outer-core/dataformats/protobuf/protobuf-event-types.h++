/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/protobuf/core/status.pb.h"  // generated from `status.proto`
#include "status/error.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
{
    //==========================================================================
    // status::Domain encoding to/decoding from cc::status::Domain

    cc::status::Domain encoded(core::status::Domain domain) noexcept;
    core::status::Domain decoded(cc::status::Domain domain) noexcept;

    void encode(core::status::Domain native, cc::status::Domain *proto) noexcept;
    void decode(cc::status::Domain proto, core::status::Domain *native) noexcept;

    //==========================================================================
    // core::status::Level encoding to/decoding from cc::status::Level

    cc::status::Level encoded(core::status::Level level) noexcept;
    core::status::Level decoded(cc::status::Level level) noexcept;

    void encode(core::status::Level native, cc::status::Level *proto) noexcept;
    void decode(cc::status::Level proto, core::status::Level *native) noexcept;

    //==========================================================================
    // core::status::Error encoding to/decoding from cc::status::Error

    void encode(const core::status::Error &native,
                cc::status::Error *proto) noexcept;

    void decode(const cc::status::Error &proto,
                core::status::Error *native) noexcept;

}  // namespace protobuf
