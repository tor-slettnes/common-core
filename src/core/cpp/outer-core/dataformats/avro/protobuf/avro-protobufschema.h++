/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.h++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-schemabuilder.h++"
#include "types/valuemap.h++"

#include <google/protobuf/descriptor.h>

#include <unordered_set>

namespace avro
{
    //--------------------------------------------------------------------------
    /// @class ProtoBufSchemaBuilder
    /// @brief Build Avro schema from ProtoBuf message type

    class ProtoBufSchemaBuilder : public RecordSchema
    {
        using This = ProtoBufSchemaBuilder;
        using Super = RecordSchema;

        using DescriptorSet = std::unordered_set<const google::protobuf::Descriptor *>;
        using SchemaMap = core::types::ValueMap<const google::protobuf::Descriptor *,
                                                SchemaWrapper>;

        friend SchemaWrapper schema_from_proto(const google::protobuf::Descriptor *);

    public:
        // @param[in] descriptor
        //     ProtoBuf message descriptor

        ProtoBufSchemaBuilder(
            const google::protobuf::Descriptor *descriptor);

    private:
        static core::types::ValueList fields(
            const google::protobuf::Descriptor *descriptor,
            DescriptorSet *seen_types);

        static RecordField field(
            const google::protobuf::FieldDescriptor *fd,
            bool is_optional,
            DescriptorSet *seen_types);

        static core::types::Value field_schema(
            const google::protobuf::FieldDescriptor *fd,
            DescriptorSet *seen_types);

        static EnumSchema enum_schema(
            const google::protobuf::EnumDescriptor *ed,
            const google::protobuf::EnumValueDescriptor *default_value);

        static MapSchema map_schema(
            const google::protobuf::Descriptor *md,
            DescriptorSet *seen_types);

        static SchemaWrapper schema_from_descriptor(
            const google::protobuf::Descriptor *descriptor,
            DescriptorSet *seen_types);
    };

    //--------------------------------------------------------------------------
    /// @brief
    ///     Get an Avro schema wrapper for a ProtoBuf message type
    /// @param[in] descriptor
    ///     ProtoBuf message descriptor
    /// @return
    ///     A new or existing `SchemaWrapper` instance.
    ///
    /// Well-known ProtoBuf message types are mapped to predefined Avro schemas.
    /// Custom types are mapped via a new or existing `ProtoBufSchemaBuilder`
    /// instance.  New instances are cached for future reuse.

    SchemaWrapper schema_from_proto(const google::protobuf::Descriptor *descriptor);

}  // namespace avro
