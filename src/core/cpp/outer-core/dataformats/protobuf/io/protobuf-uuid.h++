/// -*- c++ -*-
//==============================================================================
/// @file protobuf-uuid.h++
/// @brief Encode/decode routines for ProtoBuf universally unique identifiers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/uuid/uuid.pb.h"  // generated from `uuid.proto`
#include "types/uuid.h++"

namespace cc::protobuf
{
    void encode(const core::types::UUID &native, uuid::UUID *proto);
    void encode(const core::types::ByteVector &native, uuid::UUID *proto);
    void decode(const uuid::UUID &proto, core::types::UUID *native);

}  // namespace cc::protobuf
