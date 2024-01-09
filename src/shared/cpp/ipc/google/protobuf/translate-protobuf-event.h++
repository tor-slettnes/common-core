/// -*- c++ -*-
//==============================================================================
/// @file translate-protobuf-event.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event_types.pb.h"  // generated from `common-types.proto`
#include "status/event.h++"
#include "status/error.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace cc::protobuf
{
    //==========================================================================
    // status::Domain encoding to/decoding from CC::Status::Domain

    void encode(status::Domain domain, CC::Status::Domain *encoded) noexcept;
    void decode(CC::Status::Domain domain, status::Domain *decoded) noexcept;

    //==========================================================================
    // status::Level encoding to/decoding from CC::Status::Level

    void encode(status::Level level, CC::Status::Level *encoded) noexcept;
    void decode(CC::Status::Level level, status::Level *decoded) noexcept;

    //==========================================================================
    // status::Event encoding to/decoding from CC::Status::::EventData

    void encode(const status::Event &event, CC::Status::Details *msg) noexcept;
    void decode(const CC::Status::Details &msg, status::Event *event) noexcept;

    //==========================================================================
    // status::Error encoding to/decoding from CC::Status::::Details

    void encode(const status::Error &error, CC::Status::Details *msg) noexcept;
    void decode(const CC::Status::Details &msg, status::Error *error) noexcept;

}  // namespace cc::protobuf
