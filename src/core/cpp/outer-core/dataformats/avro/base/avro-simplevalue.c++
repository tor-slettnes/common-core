/// -*- c++ -*-
//==============================================================================
/// @file avro-base.c++
/// @brief Common functionality wrappers for SimpleValue endpoints
/// @author Tor Slettnes
//==============================================================================

#include "avro-simplevalue.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"

namespace avro
{
    SimpleValue::SimpleValue()
        : BaseValue()
    {
        checkstatus(avro_generic_null_new(&this->value));
    }

    SimpleValue::SimpleValue(bool boolean)
        : BaseValue()
    {
        checkstatus(avro_generic_boolean_new(&this->value, boolean));
    }

    SimpleValue::SimpleValue(std::int32_t input)
        : BaseValue()
    {
        checkstatus(avro_generic_int_new(&this->value, input));
    }

    SimpleValue::SimpleValue(std::int64_t input)
        : BaseValue()
    {
        checkstatus(avro_generic_long_new(&this->value, input));
    }

    SimpleValue::SimpleValue(float input)
        : BaseValue()
    {
        checkstatus(avro_generic_float_new(&this->value, input));
    }

    SimpleValue::SimpleValue(double input)
        : BaseValue()
    {
        checkstatus(avro_generic_double_new(&this->value, input));
    }

    SimpleValue::SimpleValue(const char *input)
        : BaseValue()
    {
        checkstatus(avro_generic_string_new(
            &this->value,
            input));
    }

    SimpleValue::SimpleValue(const std::string &input)
        : BaseValue()
    {
        checkstatus(avro_generic_string_new_length(
            &this->value,
            input.data(),
            input.size() + 1));
    }

    SimpleValue::SimpleValue(const std::string_view &input)
        : BaseValue()
    {
        checkstatus(avro_generic_string_new_length(
            &this->value,
            input.data(),
            input.size() + 1));
    }

    SimpleValue::SimpleValue(const core::types::Bytes &bytes)
        : BaseValue()
    {
        checkstatus(avro_generic_bytes_new(
            &this->value,
            (void *)bytes.data(),
            bytes.size()));
    }

    std::optional<int> SimpleValue::get_int() const
    {
        return avro::get_int(this->value);
    }

    std::optional<long> SimpleValue::get_long() const
    {
        return avro::get_long(this->value);
    }

    std::optional<float> SimpleValue::get_float() const
    {
        return avro::get_float(this->value);
    }

    std::optional<double> SimpleValue::get_double() const
    {
        return avro::get_double(this->value);
    }

    std::optional<bool> SimpleValue::get_boolean() const
    {
        return avro::get_boolean(this->value);
    }

    std::optional<std::string> SimpleValue::get_string() const
    {
        return avro::get_string(this->value);
    }

    std::optional<core::types::ByteVector> SimpleValue::get_bytes() const
    {
        return avro::get_bytes(this->value);
    }

    std::optional<core::types::ByteVector> SimpleValue::get_fixed() const
    {
        return avro::get_fixed(this->value);
    }

}  // namespace avro
