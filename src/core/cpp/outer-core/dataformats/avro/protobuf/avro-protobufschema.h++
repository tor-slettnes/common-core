/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.h++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-schemabuilder.h++"

#include <google/protobuf/descriptor.h>

namespace core::avro
{
    //--------------------------------------------------------------------------
    /// @class ProtoBufSchemaBuilder
    /// @brief Build Avro schema from ProtoBuf message type

    class ProtoBufSchemaBuilder : public RecordSchema
    {
        using This = ProtoBufSchemaBuilder;
        using Super = RecordSchema;

    public:
        // @param[in] descriptor
        //     ProtoBuf message descriptor

        ProtoBufSchemaBuilder(
            const google::protobuf::Descriptor *descriptor);

    private:
        static types::ValueList fields(
            const google::protobuf::Descriptor *descriptor);

        static RecordField field(
            const google::protobuf::FieldDescriptor *fd,
            bool is_optional);

        static types::Value field_schema(
            const google::protobuf::FieldDescriptor *fd);

        static EnumSchema enum_schema(
            const google::protobuf::EnumDescriptor *ed,
            const google::protobuf::EnumValueDescriptor *default_value);

        static MapSchema map_schema(
            const google::protobuf::Descriptor *md);

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

} // namespace core::avro
