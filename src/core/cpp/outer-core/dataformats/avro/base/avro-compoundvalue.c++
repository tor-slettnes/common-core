/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.c++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-compoundvalue.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "chrono/date-time.h++"
#include "parsers/json/writer.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include <iostream>

namespace avro
{
    CompoundValue::CompoundValue(avro_schema_t schema)
        : Super(),
          schema(schema),
          iface(checkstatus(avro_generic_class_from_schema(schema)))
    {
        checkstatus(avro_generic_value_new(this->iface, &this->value),
                    "avro_generic_value_new");
    }

    CompoundValue::CompoundValue(const std::string &json_schema)
        : CompoundValue(avro::schema_from_json(json_schema))
    {
    }

    CompoundValue::CompoundValue(const SchemaWrapper &wrapper)
        : CompoundValue(wrapper.as_avro_schema())
    {
    }

    CompoundValue::~CompoundValue()
    {
        avro_value_iface_decref(this->iface);
        // avro_schema_decref(schema);
    }

    avro_value_t CompoundValue::get_field_by_index(
        int index,
        const std::optional<std::string> &expected_name) const
    {
        return avro::get_field_by_index(this->value, index, expected_name);
    }

    avro_value_t CompoundValue::get_field_by_name(
        const std::string &name,
        const std::optional<std::size_t> &expected_index) const
    {
        return avro::get_field_by_name(this->value, name, expected_index);
    }

    void CompoundValue::set_from_value(
        const core::types::Value &value)
    {
        avro::set_value(&this->value, value);
    }

    void CompoundValue::set_from_serialized(
        const core::types::Bytes &bytes)
    {
        avro::set_from_serialized(&this->value, bytes);
    }

}  // namespace avro
