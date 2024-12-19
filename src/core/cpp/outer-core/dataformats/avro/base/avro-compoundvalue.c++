/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.c++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-compoundvalue.h++"
#include "avro-status.h++"
#include "parsers/json/writer.h++"

namespace core::avro
{

    CompoundValue::CompoundValue(avro_schema_t schema)
        : BaseValue(),
          schema(schema),
          iface(checkstatus(avro_generic_class_from_schema(schema)))
    {
        checkstatus(avro_generic_value_new(this->iface, &this->value));
    }

    CompoundValue::CompoundValue(const std::string &json_schema)
        : CompoundValue(This::schema_from_json(json_schema))
    {
    }

    CompoundValue::CompoundValue(const SchemaBuilder &builder)
        : CompoundValue(builder.as_avro_schema())
    {
    }

    CompoundValue::~CompoundValue()
    {
        avro_value_iface_decref(this->iface);
        avro_schema_decref(schema);
    }

    void CompoundValue::assign(const types::Value &value)
    {
    }

    avro_schema_t CompoundValue::schema_from_json(
        const std::string &json)
    {
        avro_schema_t schema;
        checkstatus(avro_schema_from_json_length(
            json.data(),
            json.size(),
            &schema));
        return schema;
    }

} // namespace core::avro
