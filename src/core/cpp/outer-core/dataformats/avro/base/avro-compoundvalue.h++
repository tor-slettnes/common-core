/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.h++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-basevalue.h++"
#include "avro-schemabuilder.h++"
#include "types/value.h++"

namespace avro
{
    class CompoundValue : public BaseValue
    {
        using This = CompoundValue;
        using Super = BaseValue;

    public:
        CompoundValue(avro_schema_t schema, bool take_schema);
        CompoundValue(const std::string &json_schema);
        CompoundValue(const SchemaWrapper &wrapper);

    public:
        ~CompoundValue();

    public:
        avro_value_t get_field_by_index(
            int index,
            const std::optional<std::string> &expected_name = {}) const;

        avro_value_t get_field_by_name(
            const std::string &name,
            const std::optional<std::size_t> &expected_index = {}) const;

        void set_from_value(const core::types::Value &value);
        void set_from_serialized(const core::types::Bytes &bytes);

    protected:
        avro_schema_t schema;
        bool take_schema;
        avro_value_iface_t *iface;
    };
}  // namespace avro
