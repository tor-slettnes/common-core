/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-event-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // core::status::Domain encoding to/decoding from cc::status::Domain

    void encode(core::status::Domain domain, cc::status::Domain *encoded) noexcept
    {
        *encoded = static_cast<cc::status::Domain>(domain);
    }

    void decode(cc::status::Domain domain, core::status::Domain *decoded) noexcept
    {
        *decoded = static_cast<core::status::Domain>(domain);
    }

    //==========================================================================
    // core::status::Level encoding to/decoding from cc::status::Level

    void encode(core::status::Level level, cc::status::Level *encoded) noexcept
    {
        *encoded = static_cast<cc::status::Level>(level);
    }

    void decode(cc::status::Level level, core::status::Level *decoded) noexcept
    {
        *decoded = static_cast<core::status::Level>(level);
    }

    //==========================================================================
    // core::status::Flow encoding to/decoding from cc::status::Flow

    void encode(core::status::Flow flow, cc::status::Flow *encoded) noexcept
    {
        *encoded = static_cast<cc::status::Flow>(flow);
    }

    void decode(cc::status::Flow flow, core::status::Flow *decoded) noexcept
    {
        *decoded = static_cast<core::status::Flow>(flow);
    }

    //==========================================================================
    // core::status::Event encoding to/decoding from core::status::Details

    void encode(const core::status::Event &event, cc::status::Event *msg) noexcept
    {
        msg->set_domain(encoded<cc::status::Domain>(event.domain()));
        msg->set_origin(event.origin());
        msg->set_code(event.code());
        msg->set_symbol(event.symbol());
        msg->set_level(encoded<cc::status::Level>(event.level()));
        msg->set_flow(encoded<cc::status::Flow>(event.flow()));
        encode(event.timepoint(), msg->mutable_timestamp());
        encode(event.attributes(), msg->mutable_attributes());
        msg->set_text(event.text());
    }

    void decode(const cc::status::Event &msg, core::status::Event *event) noexcept
    {
        *event = core::status::Event(
            msg.text(),
            decoded<core::status::Domain>(msg.domain()),
            msg.origin(),
            msg.code(),
            msg.symbol(),
            decoded<core::status::Level>(msg.level()),
            decoded<core::status::Flow>(msg.flow()),
            decoded<core::dt::TimePoint>(msg.timestamp()),
            decoded<core::types::KeyValueMap>(msg.attributes()));
    }

}  // namespace protobuf
