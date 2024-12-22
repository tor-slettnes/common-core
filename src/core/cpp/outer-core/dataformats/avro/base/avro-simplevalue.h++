/// -*- c++ -*-
//==============================================================================
/// @file avro-base.h++
/// @brief Common functionality wrappers for AVRO serialization
/// @author Tor Slettnes <tor@slett.net>
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
        SimpleValue(bool boolean);
        SimpleValue(const char *input);
        SimpleValue(const std::string &input);
        SimpleValue(const std::string_view &input);
        SimpleValue(const core::types::Bytes &bytes);

        core::types::Value as_value(bool enums_as_strings = true) const;
        std::optional<std::string> get_string() const;
        std::optional<core::types::ByteVector> get_bytes() const;
        std::optional<int> get_int() const;
        std::optional<long> get_long() const;
        std::optional<float> get_float() const;
        std::optional<double> get_double() const;
        std::optional<bool> get_boolean() const;
        std::optional<int> get_enum_value() const;
        std::optional<std::string> get_enum_symbol() const;
        std::optional<core::types::ByteVector> get_fixed() const;
        core::types::KeyValueMapPtr get_map(bool enums_as_strings) const;
        core::types::ValueListPtr get_array(bool enums_as_strings) const;
        std::optional<core::types::Value> get_union(bool enums_as_strings) const;

        template <typename EnumType>
        EnumType get_enum() const
        {
            int enumvalue = 0;
            avro_value_get_enum(this->c_value(), &enumvalue);
            return static_cast<EnumType>(enumvalue);
        }

    };
}
