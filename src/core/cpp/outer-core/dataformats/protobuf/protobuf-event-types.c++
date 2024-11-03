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

    //==========================================================================
    // core::logging::Message encoding to/decoding from cc::status::Event

    void encode(const core::logging::Message &native, cc::status::Event *proto) noexcept
    {
        encode(static_cast<core::status::Event>(native), proto);
        core::types::KeyValueMap attributes;

        attributes.insert_if(native.scope() != nullptr,
                             core::logging::MESSAGE_FIELD_LOG_SCOPE,
                             native.scopename());

        attributes.insert_if(!native.path().empty(),
                             core::logging::MESSAGE_FIELD_SOURCE_PATH,
                             native.path());

        attributes.insert_if(native.lineno(),
                             core::logging::MESSAGE_FIELD_SOURCE_LINE,
                             native.lineno());

        attributes.insert_if(!native.function().empty(),
                             core::logging::MESSAGE_FIELD_FUNCTION_NAME,
                             native.function());

        attributes.insert_if(native.thread_id(),
                             core::logging::MESSAGE_FIELD_THREAD_ID,
                             native.thread_id());

        encode(attributes, proto->mutable_attributes());
    }

    void decode(const cc::status::Event &proto, core::logging::Message *native) noexcept
    {
        auto level = decoded<core::status::Level>(proto.level());
        auto attributes = decoded<core::types::KeyValueMap>(proto.attributes());
        auto scope = attributes.extract_value(
            core::logging::MESSAGE_FIELD_LOG_SCOPE,
            "remote");

        *native = core::logging::Message(
            proto.text(),
            level,
            core::logging::Scope::create(scope.as_string(), level),
            decoded<core::dt::TimePoint>(proto.timestamp()),
            attributes.extract_value(core::logging::MESSAGE_FIELD_SOURCE_PATH).as_string(),
            attributes.extract_value(core::logging::MESSAGE_FIELD_SOURCE_LINE).as_uint(),
            attributes.extract_value(core::logging::MESSAGE_FIELD_FUNCTION_NAME).as_string(),
            static_cast<pid_t>(attributes.extract_value(core::logging::MESSAGE_FIELD_THREAD_ID).as_uint()),
            proto.origin(),
            proto.code(),
            proto.symbol(),
            attributes,
            proto.contract_id(),
            proto.host());
    }


    //==========================================================================
    // encode/decode a loggable item as either an event or a message,
    // whichever applies

    void encode_event(const core::status::Event::ptr &event, cc::status::Event *proto) noexcept
    {
        if (const auto &message = std::dynamic_pointer_cast<core::logging::Message>(event))
        {
            encode(*message, proto);
        }
        else
        {
            encode(*event, proto);
        }
    }

    cc::status::Event encoded_event(const core::status::Event::ptr &event) noexcept
    {
        cc::status::Event msg;
        encode_event(event, &msg);
        return msg;
    }

    void decode_event(const cc::status::Event &proto, core::status::Event::ptr *native) noexcept
    {
        if (proto.text().empty())
        {
            *native = std::make_shared<core::status::Event>();
            decode(proto, native->get());
        }
        else
        {
            *native = std::make_shared<core::logging::Message>(proto.text());
            decode(proto, native->get());
        }
    }

    core::status::Event::ptr decoded_event(const cc::status::Event &proto) noexcept
    {
        core::status::Event::ptr ptr;
        decode_event(proto, &ptr);
        return ptr;
    }

}  // namespace protobuf
