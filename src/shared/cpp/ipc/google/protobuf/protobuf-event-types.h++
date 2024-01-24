/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event_types.pb.h"  // generated from `common-types.proto`
#include "status/event.h++"
#include "status/event.h++"

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
    // status::Level encoding to/decoding from CC::Status::Level

    void encode(status::Flow flow, CC::Status::Flow *encoded) noexcept;
    void decode(CC::Status::Flow flow, status::Flow *decoded) noexcept;

    //==========================================================================
    // status::Event encoding to/decoding from CC::Status::::EventData

    void encode(const status::Event &event, CC::Status::Details *msg) noexcept;
    void decode(const CC::Status::Details &msg, status::Event *event) noexcept;

}  // namespace cc::protobuf
