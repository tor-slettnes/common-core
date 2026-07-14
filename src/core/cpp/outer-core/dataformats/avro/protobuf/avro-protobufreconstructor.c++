/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.c++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufreconstructor.h++"

namespace cc::avro
{
    void ProtoBufBuilder::reconstruct(
        const BaseValue &avro_value,
        ::google::protobuf::Message *msg) const
    {
        const ::google::protobuf::Descriptor *descriptor = msg->GetDescriptor();
        const SchemaWrapper &wrapper = ProtoBufSchema::from_proto(descriptor);

        if (descriptor->well_known_type())
        {
            This::reconstruct_wellknown(avro_value, msg);
        }
        else if (auto *value = dynamic_cast<cc::protobuf::variant::Value *>(msg))
        {
            This::reconstruct_variant(avro_value, value);
        }
        else if (auto *list = dynamic_cast<cc::protobuf::variant::ValueList *>(msg))
        {
            This::reconstruct_variant_list(avro_value, list);
        }
        else if (auto *map = dynamic_cast<cc::protobuf::variant::KeyValueMap *>(msg))
        {
            This::reconstruct_variant_map(avro_value, map);
        }
        else
        {
            This::reconstruct_custom(avro_value, msg);
        }
    }

    void ProtoBufBuilder::reconstruct_variant(
        const BaseValue &avro_value,
        cc::protobuf::variant::Value *value)
    {
    }

    void ProtoBufBuilder::reconstruct_variant_list(
        const BaseValue &avro_value,
        cc::protobuf::variant::ValueList *list)
    {
    }

    void ProtoBufBuilder::reconstruct_variant_map(
        const BaseValue &avro_value,
        cc::protobuf::variant::KeyValueMap *kvmap)
    {
    }

    void ProtoBufBuilder::reconstruct_wellknown(
        const BaseValue &avro_value,
        ::google::protobuf::Message *msg)
    {
    }

    void ProtoBufBuilder::reconstruct_custom(
        const BaseValue &avro_value,
        ::google::protobuf::Message *msg)
    {
    }

}  // namespace cc::avro
