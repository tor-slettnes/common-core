/// -*- c++ -*-
//==============================================================================
/// @file avro-base.h++
/// @brief Common functionality wrappers for AVRO serialization
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-basevalue.h++"

#include "types/bytevector.h++"
#include "types/value.h++"

namespace avro
{
    class SimpleValue : public BaseValue
    {
        using This = SimpleValue;
        using Super = BaseValue;

    protected:
        using BaseValue::BaseValue;

    public:
        SimpleValue();
        SimpleValue(bool boolean);
        SimpleValue(std::int32_t input);
        SimpleValue(std::int64_t input);
        SimpleValue(float input);
        SimpleValue(double input);
        SimpleValue(const char *input);
        SimpleValue(const std::string &input);
        SimpleValue(const std::string_view &input);
        SimpleValue(const core::types::Bytes &bytes);

        std::optional<int> get_int() const;
        std::optional<long> get_long() const;
        std::optional<float> get_float() const;
        std::optional<double> get_double() const;
        std::optional<bool> get_boolean() const;
        std::optional<std::string> get_string() const;
        std::optional<core::types::ByteVector> get_bytes() const;
        std::optional<core::types::ByteVector> get_fixed() const;

        template <typename EnumType>
        EnumType get_enum() const
        {
            int enumvalue = 0;
            avro_value_get_enum(this->c_value(), &enumvalue);
            return static_cast<EnumType>(enumvalue);
        }
    };
}
