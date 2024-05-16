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

namespace core::io::proto
{
    //==========================================================================
    // status::Domain encoding to/decoding from cc::status::Domain

    void encode(core::status::Domain domain, cc::status::Domain *encoded) noexcept;
    void decode(cc::status::Domain domain, core::status::Domain *decoded) noexcept;

    //==========================================================================
    // core::status::Level encoding to/decoding from cc::status::Level

    void encode(core::status::Level level, cc::status::Level *encoded) noexcept;
    void decode(cc::status::Level level, core::status::Level *decoded) noexcept;

    //==========================================================================
    // core::status::Level encoding to/decoding from cc::status::Level

    void encode(core::status::Flow flow, cc::status::Flow *encoded) noexcept;
    void decode(cc::status::Flow flow, core::status::Flow *decoded) noexcept;

    //==========================================================================
    // core::status::Event encoding to/decoding from cc::status::::EventData

    void encode(const core::status::Event &event, cc::status::Event *msg) noexcept;
    void decode(const cc::status::Event &msg, core::status::Event *event) noexcept;

}  // namespace core::io::proto
