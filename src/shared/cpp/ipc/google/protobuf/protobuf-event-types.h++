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

namespace protobuf
{
    //==========================================================================
    // status::Domain encoding to/decoding from CC::Status::Domain

    void encode(shared::status::Domain domain, CC::Status::Domain *encoded) noexcept;
    void decode(CC::Status::Domain domain, shared::status::Domain *decoded) noexcept;

    //==========================================================================
    // shared::status::Level encoding to/decoding from CC::Status::Level

    void encode(shared::status::Level level, CC::Status::Level *encoded) noexcept;
    void decode(CC::Status::Level level, shared::status::Level *decoded) noexcept;

    //==========================================================================
    // shared::status::Level encoding to/decoding from CC::Status::Level

    void encode(shared::status::Flow flow, CC::Status::Flow *encoded) noexcept;
    void decode(CC::Status::Flow flow, shared::status::Flow *decoded) noexcept;

    //==========================================================================
    // shared::status::Event encoding to/decoding from CC::Status::::EventData

    void encode(const shared::status::Event &event, CC::Status::Details *msg) noexcept;
    void decode(const CC::Status::Details &msg, shared::status::Event *event) noexcept;

}  // namespace shared::protobuf
