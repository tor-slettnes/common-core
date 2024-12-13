/// -*- c++ -*-
//==============================================================================
/// @file avro-base.c++
/// @brief Common functionality wrappers for SimpleValue endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-simplevalue.h++"

namespace core::avro
{
    SimpleValue::SimpleValue(bool boolean)
        : BaseValue()
    {
        This::checkstatus(avro_generic_boolean_new(&this->value, boolean));
    }

    SimpleValue::SimpleValue(const char *input)
        : BaseValue()
    {
        This::checkstatus(avro_generic_string_new(
            &this->value,
            input));
    }

    SimpleValue::SimpleValue(const std::string &input)
        : BaseValue()
    {
        This::checkstatus(avro_generic_string_new_length(
            &this->value,
            input.data(),
            input.size()));
    }

    SimpleValue::SimpleValue(const std::string_view &input)
        : BaseValue()
    {
        This::checkstatus(avro_generic_string_new_length(
            &this->value,
            input.data(),
            input.size()));
    }

    SimpleValue::SimpleValue(const types::Bytes &bytes)
        : BaseValue()
    {
        This::checkstatus(avro_generic_bytes_new(
            &this->value,
            (void *)bytes.data(),
            bytes.size()));
    }

} // namespace core::avro
