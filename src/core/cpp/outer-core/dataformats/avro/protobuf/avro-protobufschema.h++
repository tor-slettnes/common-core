/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufschema.h++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-schemabuilder.h++"
#include "types/valuemap.h++"

#include <google/protobuf/descriptor.h>

#include <unordered_set>

namespace cc::avro
{
    constexpr auto METADATA_FIELD = "metadata";

    //--------------------------------------------------------------------------
    /// @class ProtoBufSchema
    /// @brief Build Avro schema from ProtoBuf message type

    class ProtoBufSchema : public RecordSchema
    {
        using This = ProtoBufSchema;
        using Super = RecordSchema;

        using DescriptorSet = std::unordered_set<const google::protobuf::Descriptor *>;
        using NameTranslationMap = core::types::ValueMap<std::string, std::string>;

        struct FieldData
        {
            SchemaWrapper schema;
            std::optional<core::types::Value> default_value;
        };

    protected:
        // @param[in] descriptor
        //     ProtoBuf message descriptor

        ProtoBufSchema(
            const ContextRef &context,
            const google::protobuf::Descriptor *contents,
            const google::protobuf::Descriptor *metadata = nullptr);

    public:
        //--------------------------------------------------------------------------
        /// @brief
        ///     Get an Avro schema wrapper for a ProtoBuf message type
        /// @param[in] descriptor
        ///     ProtoBuf message descriptor
        /// @return
        ///     A new or existing `SchemaWrapper` instance.
        ///
        /// Well-known ProtoBuf message types are mapped to predefined Avro schemas.
        /// Custom types are mapped via a new or existing `ProtoBufSchema`
        /// instance.  New instances are cached for future reuse.

        static const SchemaWrapper &from_proto(
            const google::protobuf::Descriptor *contents,
            const google::protobuf::Descriptor *metadata = nullptr);

    private:
        void add_fields();

        FieldData field(
            const google::protobuf::FieldDescriptor *fd);

        FieldData field_schema(
            const google::protobuf::FieldDescriptor *fd);

        FieldData enum_schema(
            const google::protobuf::EnumDescriptor *ed,
            const google::protobuf::EnumValueDescriptor *default_value);

        FieldData map_schema(
            const google::protobuf::Descriptor *md);

        static FieldData from_descriptor(
            const ContextRef &context,
            const google::protobuf::Descriptor *descriptor);

        static std::optional<std::string> field_comment(
            const google::protobuf::FieldDescriptor *fd);

    public:
        static std::string schema_name(
            const google::protobuf::Descriptor *descriptor);

        static std::string translated_namespace(
            const std::string &protobuf_namespace);

    private:
        const google::protobuf::Descriptor *descriptor;
        const google::protobuf::Descriptor *metadata;
    };

}  // namespace cc::avro
