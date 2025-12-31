/// -*- c++ -*-
//==============================================================================
/// @file protobuf-event-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-event-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // core::status::Domain encoding to/decoding from cc::protobuf::status::Domain

    cc::protobuf::status::Domain encoded(core::status::Domain domain) noexcept
    {
        return static_cast<cc::protobuf::status::Domain>(domain);
    }

    core::status::Domain decoded(cc::protobuf::status::Domain domain) noexcept
    {
        return static_cast<core::status::Domain>(domain);
    }

    void encode(core::status::Domain native, cc::protobuf::status::Domain *proto) noexcept
    {
        *proto = encoded(native);
    }

    void decode(cc::protobuf::status::Domain proto, core::status::Domain *native) noexcept
    {
        *native = decoded(proto);
    }

    //==========================================================================
    // core::status::Level encoding to/decoding from cc::protobuf::status::Level

    cc::protobuf::status::Level encoded(core::status::Level level) noexcept
    {
        return static_cast<cc::protobuf::status::Level>(level);
    }

    core::status::Level decoded(cc::protobuf::status::Level level) noexcept
    {
        return static_cast<core::status::Level>(level);
    }

    void encode(core::status::Level native, cc::protobuf::status::Level *proto) noexcept
    {
        *proto = encoded(native);
    }

    void decode(cc::protobuf::status::Level proto, core::status::Level *native) noexcept
    {
        *native = decoded(proto);
    }

    //==========================================================================
    // core::status::Error encoding to/decoding from cc::protobuf::status::Error

    void encode(const core::status::Error &native,
                cc::protobuf::status::Error *proto) noexcept
    {
        proto->set_domain(encoded(native.domain()));
        proto->set_origin(native.origin());
        proto->set_level(encoded(native.level()));
        proto->set_code(native.code());
        proto->set_symbol(native.symbol());
        encode(native.timepoint(), proto->mutable_timestamp());
        encode(native.attributes(), proto->mutable_attributes());
        proto->set_text(native.text());
    }

    void decode(const cc::protobuf::status::Error &proto,
                core::status::Error *native) noexcept
    {
        *native = core::status::Error(
            proto.text(),
            decoded(proto.domain()),
            proto.origin(),
            proto.code(),
            proto.symbol(),
            decoded(proto.level()),
            decoded<core::dt::TimePoint>(proto.timestamp()),
            decoded<core::types::KeyValueMap>(proto.attributes()));
    }

}  // namespace protobuf
