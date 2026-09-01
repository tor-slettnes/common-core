/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.c++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes
//==============================================================================

#include "avro-basevalue.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "string/misc.h++"

#include <stdlib.h>

#include <algorithm>
#include <type_traits>

namespace cc::avro
{
    BaseValue::~BaseValue()
    {
        avro_value_decref(&this->avro_value_);
    }

    bool BaseValue::operator==(const BaseValue &other) const
    {
        return this->operator==(other.avro_value());
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
        return &this->avro_value_;
    }

    const avro_value_t *BaseValue::c_value() const
    {
        return &this->avro_value_;
    }

    avro_value_t &BaseValue::avro_value()
    {
        return this->avro_value_;
    }

    const avro_value_t &BaseValue::avro_value() const
    {
        return this->avro_value_;
    }

    const avro_schema_t &BaseValue::avro_schema() const
    {
        return avro_value_get_schema(&this->avro_value_);
    }

    avro_type_t BaseValue::avro_type() const
    {
        return avro_value_get_type(&this->avro_value_);
    }

    std::string BaseValue::avro_type_name() const
    {
        return avro_schema_type_name(this->avro_schema());
    }

    std::size_t BaseValue::serialized_size() const
    {
        std::size_t size = 0;
        checkstatus(
            avro_value_sizeof(
                const_cast<avro_value_t *>(&this->avro_value_),
                &size));

        return size;
    }

    core::types::ByteVector BaseValue::serialized() const
    {
        std::size_t nbytes = this->serialized_size();
        core::types::ByteVector buffer(nbytes);

        avro_writer_t writer = avro_writer_memory(
            reinterpret_cast<char *>(buffer.data()),
            buffer.size());

        avro_value_write(
            writer,
            const_cast<avro_value_t *>(&this->avro_value_));

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
        checkstatus(
            avro_value_to_json(
                &this->avro_value_,
                !pretty,
                &json_str));

        std::string result(json_str);
        free(json_str);
        return result;
    }

    core::types::Value BaseValue::as_value(bool enums_as_strings) const
    {
        return avro::get_value(this->avro_value_, enums_as_strings);
    }

}  // namespace cc::avro
