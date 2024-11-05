/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-event-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-message.h++"
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
    // core::status::Event encoding to/decoding from core::status::Details

    void encode(const core::status::Event &native, cc::status::Event *proto) noexcept
    {
        proto->set_text(native.text());
        proto->set_domain(encoded<cc::status::Domain>(native.domain()));
        proto->set_origin(native.origin());
        proto->set_code(native.code());
        proto->set_symbol(native.symbol());
        proto->set_level(encoded<cc::status::Level>(native.level()));
        encode(native.timepoint(), proto->mutable_timestamp());
        encode(native.attributes(), proto->mutable_attributes());
        proto->set_contract_id(native.contract_id());
        proto->set_host(native.host());
    }

    void decode(const cc::status::Event &proto, core::status::Event *native) noexcept
    {
        *native = core::status::Event(
            proto.text(),
            decoded<core::status::Domain>(proto.domain()),
            proto.origin(),
            proto.code(),
            proto.symbol(),
            decoded<core::status::Level>(proto.level()),
            decoded<core::dt::TimePoint>(proto.timestamp()),
            decoded<core::types::KeyValueMap>(proto.attributes()),
            proto.contract_id(),
            proto.host());
    }

    void encode(core::status::Event::ptr native, cc::status::Event *proto) noexcept
    {
        if (auto message = std::dynamic_pointer_cast<core::logging::Message>(native))
        {
            native = message->as_event();
        }

        if (native)
        {
            encode(*native, proto);
        }
    }

    void decode(const cc::status::Event &proto,
                core::status::Event::ptr *native) noexcept
    {
        core::status::Event event;
        decode(proto, &event);

        if (auto message = core::logging::Message::create_from_event(event))
        {
            *native = message;
        }
        else
        {
            *native = std::make_shared<core::status::Event>(std::move(event));
        }
    }

}  // namespace protobuf
