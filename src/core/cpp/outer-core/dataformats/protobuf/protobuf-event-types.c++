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

    void encode(const core::logging::Message &native, cc::status::Event *proto) noexcept
    {
        encode(static_cast<const core::status::Event>(native), proto);
        proto->set_thread_id(native.thread_id());
        proto->set_log_scope(native.scopename());
        proto->set_source_path(native.path());
        proto->set_source_line(native.lineno());
        proto->set_function_name(native.function());
    }

    void decode(const cc::status::Event &proto, core::logging::Message *native) noexcept
    {
        core::status::Level level = decoded<core::status::Level>(proto.level());

        core::logging::Scope::ptr scope =
            proto.log_scope().empty()
                ? log_scope
                : core::logging::Scope::create(proto.log_scope(), level);

        *native = core::logging::Message(
            proto.text(),                                           // text
            level,                                                  // level
            scope,                                                  // scope
            decoded<core::dt::TimePoint>(proto.timestamp()),        // tp
            proto.source_path(),                                    // path
            proto.source_line(),                                    // lineno
            proto.function_name(),                                  // function
            proto.thread_id(),                                      // thread_id
            decoded<core::status::Domain>(proto.domain()),          // domain
            proto.origin(),                                         // origin
            proto.code(),                                           // code
            proto.symbol(),                                         // symbol
            decoded<core::types::KeyValueMap>(proto.attributes()),  // attributes
            proto.contract_id(),                                    // contract_id
            proto.host());                                          // host
    }

    void encode(core::status::Event::ptr native_ptr, cc::status::Event *proto) noexcept
    {
        if (native_ptr)
        {
            if (auto message_ptr = std::dynamic_pointer_cast<core::logging::Message>(native_ptr))
            {
                encode(*message_ptr, proto);
            }
            else
            {
                encode(*native_ptr, proto);
            }
        }
    }

    void decode(const cc::status::Event &proto, core::status::Event::ptr *native_ptr) noexcept
    {
        if (!proto.text().empty())
        {
            *native_ptr = decode_shared<core::logging::Message>(proto);
        }
        else
        {
            *native_ptr = decode_shared<core::status::Event>(proto);
        }
    }
}  // namespace protobuf
