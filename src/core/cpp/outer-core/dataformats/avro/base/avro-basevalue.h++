/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.h++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"

#include <avro.h>

#include <memory>
#include <stdexcept>

namespace core::avro
{
    class BaseValue
    {
        using This = BaseValue;

    protected:
        // Empty constructor invoked only through subclasses.  It is the
        // responsibility of the derived class to create the initial Avro value
        // reference.
        BaseValue() {}

    public:
        /// @brief
        ///     RAII constructor for C `avro_value_t` instances.
        /// @param[in] c_value
        ///     `avro_value_t` pointer

        BaseValue(const avro_value_t &avro_value);
        BaseValue(avro_value_t &&avro_value);

        BaseValue(const BaseValue &other);
        BaseValue(BaseValue &&other);

        virtual ~BaseValue();

        BaseValue &operator=(const BaseValue &other);
        BaseValue &operator=(BaseValue &&other);

    public:
        const avro_value_t *c_value() const;
        avro_type_t avro_type() const;
        avro_schema_t avro_schema() const;

        types::ValueType value_type() const;
        types::Value as_value(bool enums_as_strings = true) const;

    public:
        std::optional<std::string> get_string() const;
        std::optional<types::ByteVector> get_bytes() const;
        std::optional<int> get_int() const;
        std::optional<long> get_long() const;
        std::optional<float> get_float() const;
        std::optional<double> get_double() const;
        std::optional<bool> get_boolean() const;
        std::optional<int> get_enum_value() const;
        std::optional<std::string> get_enum_symbol() const;
        std::optional<types::ByteVector> get_fixed() const;
        types::KeyValueMapPtr get_map(bool enums_as_strings) const;
        types::ValueListPtr get_array(bool enums_as_strings) const;
        std::optional<types::Value> get_union(bool enums_as_strings) const;

        template <typename EnumType>
        EnumType get_enum() const
        {
            int enumvalue = 0;
            avro_value_get_enum(this->c_value(), &enumvalue);
            return static_cast<EnumType>(enumvalue);
        }

    protected:
        static std::runtime_error error(const std::string &context);
        static int checkstatus(int status, const std::string &context = {});

        template <class PT>
        static PT *checkstatus(PT *pointer, const std::string &context = {})
        {
            if (pointer == nullptr)
            {
                throw This::error(context);
            }
            return pointer;
        }

    protected:
        avro_value_t value;
    };
} // namespace core::avro
