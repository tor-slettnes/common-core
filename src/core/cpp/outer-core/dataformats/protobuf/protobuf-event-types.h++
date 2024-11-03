/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status.pb.h"  // generated from `status.proto`
#include "status/event.h++"
#include "logging/message/message.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
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
    // core::status::Event encoding to/decoding from cc::status::Event

    void encode(const core::status::Event &native, cc::status::Event *proto) noexcept;
    void decode(const cc::status::Event &proto, core::status::Event *native) noexcept;

    //==========================================================================
    // core::logging::Message encoding to/decoding from cc::status::Event

    void encode(const core::logging::Message &native, cc::status::Event *proto) noexcept;
    void decode(const cc::status::Event &proto, core::logging::Message *native) noexcept;


    //==========================================================================
    // encode/decode a loggable item as either an event or a message,
    // whichever applies

    void encode_event(const core::status::Event::ptr &event, cc::status::Event *proto) noexcept;
    cc::status::Event encoded_event(const core::status::Event::ptr &event) noexcept;

    void decode_event(const cc::status::Event &proto, core::status::Event::ptr *event) noexcept;
    core::status::Event::ptr decoded_event(const cc::status::Event &proto) noexcept;

}  // namespace protobuf
