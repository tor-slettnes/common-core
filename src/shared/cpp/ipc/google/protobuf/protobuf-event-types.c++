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

namespace cc::protobuf
{
    //==========================================================================
    // status::Domain encoding to/decoding from CC::Status::Domain

    void encode(status::Domain domain, CC::Status::Domain *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Domain>(domain);
    }

    void decode(CC::Status::Domain domain, status::Domain *decoded) noexcept
    {
        *decoded = static_cast<status::Domain>(domain);
    }

    //==========================================================================
    // status::Level encoding to/decoding from CC::Status::Level

    void encode(status::Level level, CC::Status::Level *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Level>(level);
    }

    void decode(CC::Status::Level level, status::Level *decoded) noexcept
    {
        *decoded = static_cast<status::Level>(level);
    }

    //==========================================================================
    // status::Flow encoding to/decoding from CC::Status::Flow

    void encode(status::Flow flow, CC::Status::Flow *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Flow>(flow);
    }

    void decode(CC::Status::Flow flow, status::Flow *decoded) noexcept
    {
        *decoded = static_cast<status::Flow>(flow);
    }

    //==========================================================================
    // status::Event encoding to/decoding from status::Details

    void encode(const status::Event &event, CC::Status::Details *msg) noexcept
    {
        msg->set_domain(encoded<CC::Status::Domain>(event.domain()));
        msg->set_origin(event.origin());
        msg->set_code(event.code());
        msg->set_symbol(event.symbol());
        msg->set_level(encoded<CC::Status::Level>(event.level()));
        msg->set_flow(encoded<CC::Status::Flow>(event.flow()));
        encode(event.timepoint(), msg->mutable_timestamp());
        encode(event.attributes(), msg->mutable_attributes());
        msg->set_text(event.text());
    }

    void decode(const CC::Status::Details &msg, status::Event *event) noexcept
    {
        *event = status::Event(
            msg.text(),
            decoded<status::Domain>(msg.domain()),
            msg.origin(),
            msg.code(),
            msg.symbol(),
            decoded<status::Level>(msg.level()),
            decoded<status::Flow>(msg.flow()),
            decoded<dt::TimePoint>(msg.timestamp()),
            decoded<types::KeyValueMap>(msg.attributes()));
    }

}  // namespace cc::protobuf