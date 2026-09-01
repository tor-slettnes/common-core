/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.c++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-compoundvalue.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "chrono/date-time.h++"
#include "parsers/json/writer.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include <iostream>

namespace cc::avro
{
    CompoundValue::CompoundValue(avro_schema_t schema, bool take_schema)
        : Super(),
          schema(schema),
          take_schema(take_schema),
          iface(checkstatus(avro_generic_class_from_schema(schema)))
    {
        if (!take_schema)
        {
            avro_schema_incref(schema);
        }
        checkstatus(avro_generic_value_new(this->iface, this->c_value()),
                    "avro_generic_value_new");
    }

    CompoundValue::CompoundValue(const std::string &json_schema)
        : CompoundValue(avro::schema_from_json(json_schema), true)
    {
    }

    CompoundValue::CompoundValue(const SchemaWrapper &wrapper)
        : CompoundValue(wrapper.as_avro_schema(), false)
    {
    }

    CompoundValue::CompoundValue(const avro_value_t &avro_value)
        : CompoundValue(avro_value_get_schema(&avro_value), false)
    {
        checkstatus(avro_value_copy(this->c_value(), &avro_value));
    }

    CompoundValue::CompoundValue(const BaseValue &other)
        : CompoundValue(other.avro_schema(), false)
    {
        checkstatus(avro_value_copy(this->c_value(), other.c_value()));
    }

    CompoundValue::~CompoundValue()
    {
        avro_value_iface_decref(this->iface);
        avro_schema_decref(schema);
    }

    CompoundValue &CompoundValue::operator=(const BaseValue &other)
    {
        checkstatus(avro_value_copy(this->c_value(), other.c_value()));
        return *this;
    }

    avro_value_t CompoundValue::get_field_by_index(
        int index,
        const std::optional<std::string> &expected_name) const
    {
        return avro::get_field_by_index(this->avro_value(), index, expected_name);
    }

    avro_value_t CompoundValue::get_field_by_name(
        const std::string &name,
        const std::optional<std::size_t> &expected_index) const
    {
        return avro::get_field_by_name(this->avro_value(), name, expected_index);
    }

    void CompoundValue::set_from_value(
        const core::types::Value &value)
    {
        avro::set_value(this->c_value(), value);
    }

    void CompoundValue::set_from_serialized(
        const core::types::Bytes &bytes)
    {
        avro::set_from_serialized(this->c_value(), bytes);
    }

}  // namespace cc::avro
