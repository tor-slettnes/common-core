/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.c++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-basevalue.h++"
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

    avro_value_t *BaseValue::c_value()
    {
        return &this->value;
    }

    const avro_value_t *BaseValue::c_value() const
    {
        return &this->value;
    }

    avro_type_t BaseValue::avro_type() const
    {
        return avro_value_get_type(&this->value);
    }

    avro_schema_t BaseValue::avro_schema() const
    {
        return avro_value_get_schema(&this->value);
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

    std::string BaseValue::as_json(bool pretty) const
    {
        char *json_str = nullptr;
        checkstatus(avro_value_to_json(&this->value, !pretty, &json_str));
        std::string result(json_str);
        free(json_str);
        return result;
    }

    void BaseValue::to_stream(std::ostream &stream) const
    {
        stream << this->as_json();
    }

    avro_type_t BaseValue::type(avro_value_t *value)
    {
        return avro_value_get_type(value);
    }

    avro_schema_t BaseValue::schema(avro_value_t *value)
    {
        return avro_value_get_schema(value);
    }

    std::string BaseValue::type_name(avro_value_t *value)
    {
        return avro_schema_type_name(This::schema(value));
    }

    void BaseValue::set_null(avro_value_t *value)
    {
        checkstatus(avro_value_set_null(value));
    }

    void BaseValue::set_int(avro_value_t *value,
                            int intvalue)
    {
        checkstatus(avro_value_set_int(value, intvalue));
    }

    void BaseValue::set_long(avro_value_t *value,
                             long longvalue)
    {
        checkstatus(avro_value_set_long(value, longvalue));
    }

    void BaseValue::set_float(avro_value_t *value,
                              float floatvalue)
    {
        checkstatus(avro_value_set_float(value, floatvalue));
    }

    void BaseValue::set_double(avro_value_t *value,
                               double doublevalue)
    {
        checkstatus(avro_value_set_double(value, doublevalue));
    }

    void BaseValue::set_boolean(avro_value_t *value,
                                bool boolvalue)
    {
        checkstatus(avro_value_set_boolean(value, boolvalue));
    }

    void BaseValue::set_enum(avro_value_t *value,
                             int enumvalue)
    {
        checkstatus(avro_value_set_enum(value, enumvalue));
    }

    void BaseValue::set_string(avro_value_t *value,
                               const std::string &string)
    {
        checkstatus(avro_value_set_string_len(
            value,
            string.c_str(),
            string.size() + 1)); // Length needs to include trailing null terminator
    }

    void BaseValue::set_bytes(avro_value_t *value,
                              const core::types::Bytes &bytes)
    {
        checkstatus(avro_value_set_bytes(
            value,
            const_cast<core::types::Byte *>(bytes.data()),
            bytes.size()));
    }

    void BaseValue::set_bytes(avro_value_t *value,
                              const std::string &bytes)
    {
        checkstatus(avro_value_set_bytes(
            value,
            const_cast<char *>(bytes.data()),
            bytes.size()));
    }

    void BaseValue::set_fixed(avro_value_t *value,
                              const core::types::Bytes &bytes,
                              const std::optional<std::size_t> &nbytes)
    {
        checkstatus(avro_value_set_fixed(
            value,
            const_cast<core::types::Byte *>(bytes.data()),
            nbytes.value_or(bytes.size())));
    }

}  // namespace avro
