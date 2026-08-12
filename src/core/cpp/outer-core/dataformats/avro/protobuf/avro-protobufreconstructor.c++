/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.c++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufreconstructor.h++"
#include "protobuf-message-encoder.h++"

namespace cc::avro
{
    bool reconstruct_proto(
        const BaseValue& avro_value,
        google::protobuf::Message* msg)
    {
        return cc::protobuf::encode_to_message(
            avro_value.as_value(),
            msg);
    }
}  // namespace cc::avro
