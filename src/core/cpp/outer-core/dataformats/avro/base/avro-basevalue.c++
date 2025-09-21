/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.c++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-basevalue.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "string/misc.h++"

#include <stdlib.h>

#include <algorithm>
#include <type_traits>

namespace avro
{
    BaseValue::BaseValue(const avro_value_t &avro_value)
    {
        checkstatus(avro_value_copy(&this->value, &avro_value));
    }

    BaseValue::BaseValue(avro_value_t &&avro_value)
    {
        std::swap(this->value, avro_value);
    }

    BaseValue::BaseValue(const BaseValue &other)
    {
        checkstatus(avro_value_copy(&this->value, &other.value));
    }

    BaseValue::BaseValue(BaseValue &&other)
    {
        std::swap(this->value, other.value);
    }

    BaseValue::~BaseValue()
    {
        avro_value_decref(&this->value);
    }

    BaseValue &BaseValue::operator=(const BaseValue &other)
    {
        avro_value_decref(&this->value);
        checkstatus(avro_value_copy(&this->value, &other.value));
        return *this;
    }

    BaseValue &BaseValue::operator=(BaseValue &&other)
    {
        std::swap(this->value, other.value);
        return *this;
    }

    bool BaseValue::operator==(const BaseValue &other) const
    {
        return this->operator==(*other.c_value());
    }

    bool BaseValue::operator==(const avro_value_t &other) const
    {
        return avro_value_equal(
                   const_cast<avro_value_t *>(this->c_value()),
                   const_cast<avro_value_t *>(&other));
    }

    void BaseValue::to_stream(std::ostream &stream) const
    {
        stream << this->as_json();
    }

    avro_value_t *BaseValue::c_value()
    {
        return &this->value;
    }

    const avro_value_t *BaseValue::c_value() const
    {
        return &this->value;
    }

    avro_value_t &BaseValue::avro_value()
    {
        return this->value;
    }

    const avro_value_t &BaseValue::avro_value() const
    {
        return this->value;
    }

    const avro_schema_t &BaseValue::avro_schema() const
    {
        return avro_value_get_schema(&this->value);
    }

    avro_type_t BaseValue::avro_type() const
    {
        return avro_value_get_type(&this->value);
    }

    std::string BaseValue::avro_type_name() const
    {
        return avro_schema_type_name(this->avro_schema());
    }

    std::size_t BaseValue::serialized_size() const
    {
        std::size_t size = 0;
        checkstatus(avro_value_sizeof(const_cast<avro_value_t *>(&this->value), &size));
        return size;
    }

    core::types::ByteVector BaseValue::serialized() const
    {
        std::size_t nbytes = this->serialized_size();
        core::types::ByteVector buffer(nbytes);

        avro_writer_t writer = avro_writer_memory(
            reinterpret_cast<char *>(buffer.data()),
            buffer.size());
        avro_value_write(writer, const_cast<avro_value_t *>(&this->value));
        avro_writer_free(writer);

        return buffer;
    }

    std::shared_ptr<core::types::ByteVector> BaseValue::serialized_ptr() const
    {
        auto buffer = std::make_shared<core::types::ByteVector>();
        *buffer = this->serialized();
        return buffer;
    }

    std::string BaseValue::as_json(bool pretty) const
    {
        char *json_str = nullptr;
        checkstatus(avro_value_to_json(&this->value, !pretty, &json_str));
        std::string result(json_str);
        free(json_str);
        return result;
    }

    core::types::Value BaseValue::as_value(bool enums_as_strings) const
    {
        return avro::get_value(this->value, enums_as_strings);
    }

}  // namespace avro
