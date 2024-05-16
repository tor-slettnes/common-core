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

namespace cc::io::proto
{
    //==========================================================================
    // cc::status::Domain encoding to/decoding from cc::protobuf::status::Domain

    void encode(cc::status::Domain domain, cc::protobuf::status::Domain *encoded) noexcept
    {
        *encoded = static_cast<cc::protobuf::status::Domain>(domain);
    }

    void decode(cc::protobuf::status::Domain domain, cc::status::Domain *decoded) noexcept
    {
        *decoded = static_cast<cc::status::Domain>(domain);
    }

    //==========================================================================
    // cc::status::Level encoding to/decoding from cc::protobuf::status::Level

    void encode(cc::status::Level level, cc::protobuf::status::Level *encoded) noexcept
    {
        *encoded = static_cast<cc::protobuf::status::Level>(level);
    }

    void decode(cc::protobuf::status::Level level, cc::status::Level *decoded) noexcept
    {
        *decoded = static_cast<cc::status::Level>(level);
    }

    //==========================================================================
    // cc::status::Flow encoding to/decoding from cc::protobuf::status::Flow

    void encode(cc::status::Flow flow, cc::protobuf::status::Flow *encoded) noexcept
    {
        *encoded = static_cast<cc::protobuf::status::Flow>(flow);
    }

    void decode(cc::protobuf::status::Flow flow, cc::status::Flow *decoded) noexcept
    {
        *decoded = static_cast<cc::status::Flow>(flow);
    }

    //==========================================================================
    // cc::status::Event encoding to/decoding from cc::status::Details

    void encode(const cc::status::Event &event, cc::protobuf::status::Event *msg) noexcept
    {
        msg->set_domain(encoded<cc::protobuf::status::Domain>(event.domain()));
        msg->set_origin(event.origin());
        msg->set_code(event.code());
        msg->set_symbol(event.symbol());
        msg->set_level(encoded<cc::protobuf::status::Level>(event.level()));
        msg->set_flow(encoded<cc::protobuf::status::Flow>(event.flow()));
        encode(event.timepoint(), msg->mutable_timestamp());
        encode(event.attributes(), msg->mutable_attributes());
        msg->set_text(event.text());
    }

    void decode(const cc::protobuf::status::Event &msg, cc::status::Event *event) noexcept
    {
        *event = cc::status::Event(
            msg.text(),
            decoded<cc::status::Domain>(msg.domain()),
            msg.origin(),
            msg.code(),
            msg.symbol(),
            decoded<cc::status::Level>(msg.level()),
            decoded<cc::status::Flow>(msg.flow()),
            decoded<cc::dt::TimePoint>(msg.timestamp()),
            decoded<cc::types::KeyValueMap>(msg.attributes()));
    }

}  // namespace cc::io::proto
