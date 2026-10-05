/// -*- c++ -*-
//==============================================================================
/// @file protobuf-uuid.c++
/// @brief Encode/decode routines for ProtoBuf universally unique identifiers
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-uuid.h++"

namespace cc::protobuf
{
    void encode(const core::types::UUID &native, uuid::UUID *proto)
    {
        proto->set_value(native.data(), native.size());
    }

    void encode(const core::types::ByteVector &native, uuid::UUID *proto)
    {
        proto->set_value(native.data(), native.size());
    }

    void decode(const uuid::UUID &proto, core::types::UUID *native)
    {
        native->populate_from_bytevector(proto.value());
    }
}  // namespace cc::protobuf
