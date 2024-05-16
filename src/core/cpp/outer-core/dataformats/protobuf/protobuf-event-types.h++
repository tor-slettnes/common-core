/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status.pb.h"  // generated from `status.proto`
#include "status/event.h++"
#include "status/event.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace cc::io::proto
{
    //==========================================================================
    // status::Domain encoding to/decoding from cc::protobuf::status::Domain

    void encode(cc::status::Domain domain, cc::protobuf::status::Domain *encoded) noexcept;
    void decode(cc::protobuf::status::Domain domain, cc::status::Domain *decoded) noexcept;

    //==========================================================================
    // cc::status::Level encoding to/decoding from cc::protobuf::status::Level

    void encode(cc::status::Level level, cc::protobuf::status::Level *encoded) noexcept;
    void decode(cc::protobuf::status::Level level, cc::status::Level *decoded) noexcept;

    //==========================================================================
    // cc::status::Level encoding to/decoding from cc::protobuf::status::Level

    void encode(cc::status::Flow flow, cc::protobuf::status::Flow *encoded) noexcept;
    void decode(cc::protobuf::status::Flow flow, cc::status::Flow *decoded) noexcept;

    //==========================================================================
    // cc::status::Event encoding to/decoding from cc::protobuf::status::::EventData

    void encode(const cc::status::Event &event, cc::protobuf::status::Event *msg) noexcept;
    void decode(const cc::protobuf::status::Event &msg, cc::status::Event *event) noexcept;

}  // namespace cc::io::proto
