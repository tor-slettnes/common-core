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
    //--------------------------------------------------------------------------
    /// @class ProtoBufSchema
    /// @brief Build Avro schema from ProtoBuf message type

    class ProtoBufSchema : public RecordSchema
    {
        using This = ProtoBufSchema;
        using Super = RecordSchema;

        using DescriptorSet = std::unordered_set<const google::protobuf::Descriptor *>;
        using SchemaMap = core::types::ValueMap<const google::protobuf::Descriptor *,
                                                SchemaWrapper>;
        using NameTranslationMap = core::types::ValueMap<std::string, std::string>;

    public:
        // @param[in] descriptor
        //     ProtoBuf message descriptor

        ProtoBufSchema(
            const ContextRef &context,
            const google::protobuf::Descriptor *descriptor,
            const std::optional<std::string> &name = {});

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

        static SchemaWrapper &from_proto(
            const google::protobuf::Descriptor *descriptor,
            const std::optional<std::string> &name = {});

    private:
        void add_fields();

        core::types::Value field(
            const google::protobuf::FieldDescriptor *fd) const;

        core::types::Value field_schema(
            const google::protobuf::FieldDescriptor *fd) const;

        EnumSchema enum_schema(
            const google::protobuf::EnumDescriptor *ed,
            const google::protobuf::EnumValueDescriptor *default_value) const;

        MapSchema map_schema(
            const google::protobuf::Descriptor *md) const;

        static SchemaWrapper from_descriptor(
            const ContextRef &context,
            const google::protobuf::Descriptor *descriptor,
            const std::optional<std::string> &name={});

        static std::optional<std::string> field_comment(
            const google::protobuf::FieldDescriptor *fd);

    private:
        const google::protobuf::Descriptor *descriptor;
    };

}  // namespace cc::avro
