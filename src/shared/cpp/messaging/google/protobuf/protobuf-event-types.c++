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
    // shared::status::Domain encoding to/decoding from CC::Status::Domain

    void encode(shared::status::Domain domain, CC::Status::Domain *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Domain>(domain);
    }

    void decode(CC::Status::Domain domain, shared::status::Domain *decoded) noexcept
    {
        *decoded = static_cast<shared::status::Domain>(domain);
    }

    //==========================================================================
    // shared::status::Level encoding to/decoding from CC::Status::Level

    void encode(shared::status::Level level, CC::Status::Level *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Level>(level);
    }

    void decode(CC::Status::Level level, shared::status::Level *decoded) noexcept
    {
        *decoded = static_cast<shared::status::Level>(level);
    }

    //==========================================================================
    // shared::status::Flow encoding to/decoding from CC::Status::Flow

    void encode(shared::status::Flow flow, CC::Status::Flow *encoded) noexcept
    {
        *encoded = static_cast<CC::Status::Flow>(flow);
    }

    void decode(CC::Status::Flow flow, shared::status::Flow *decoded) noexcept
    {
        *decoded = static_cast<shared::status::Flow>(flow);
    }

    //==========================================================================
    // shared::status::Event encoding to/decoding from shared::status::Details

    void encode(const shared::status::Event &event, CC::Status::Details *msg) noexcept
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

    void decode(const CC::Status::Details &msg, shared::status::Event *event) noexcept
    {
        *event = shared::status::Event(
            msg.text(),
            decoded<shared::status::Domain>(msg.domain()),
            msg.origin(),
            msg.code(),
            msg.symbol(),
            decoded<shared::status::Level>(msg.level()),
            decoded<shared::status::Flow>(msg.flow()),
            decoded<shared::dt::TimePoint>(msg.timestamp()),
            decoded<shared::types::KeyValueMap>(msg.attributes()));
    }

}  // namespace shared::protobuf
