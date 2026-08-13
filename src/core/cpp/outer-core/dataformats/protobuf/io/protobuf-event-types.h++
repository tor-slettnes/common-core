/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/status/status.pb.h"  // generated from `status.proto`
#include "status/error.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace cc::protobuf
{
    //==========================================================================
    // core::status::Domain encoding to/decoding from core::status::Domain

    protobuf::status::Domain encoded(core::status::Domain domain) noexcept;
    core::status::Domain decoded(protobuf::status::Domain domain) noexcept;

    void encode(core::status::Domain native,
                protobuf::status::Domain *proto) noexcept;

    void decode(protobuf::status::Domain proto,
                core::status::Domain *native) noexcept;

    //==========================================================================
    // core::status::Level encoding to/decoding from protobuf::status::Level

    protobuf::status::Level encoded(core::status::Level level) noexcept;
    core::status::Level decoded(protobuf::status::Level level) noexcept;

    void encode(core::status::Level native,
                protobuf::status::Level *proto) noexcept;

    void decode(protobuf::status::Level proto,
                core::status::Level *native) noexcept;

    //==========================================================================
    // core::status::Error encoding to/decoding from protobuf::status::Error

    void encode(const core::status::Error &native,
                protobuf::status::Error *proto) noexcept;

    void decode(const protobuf::status::Error &proto,
                core::status::Error *native) noexcept;

}  // namespace cc::protobuf
