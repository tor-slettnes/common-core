/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.h++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-basevalue.h++"
#include <google/protobuf/message.h>

namespace cc::avro
{
    bool reconstruct_proto(
        const BaseValue& avro_value,
        google::protobuf::Message* msg);
}  // namespace cc::avro
