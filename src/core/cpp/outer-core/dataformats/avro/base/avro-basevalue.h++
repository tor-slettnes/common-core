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

namespace avro
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

        core::types::ValueType value_type() const;
        core::types::Value as_value(bool enums_as_strings = true) const;
        std::string as_json(bool pretty = false) const;

    public:
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

    protected:
        static avro_type_t type(avro_value_t *value);
        static avro_schema_t schema(avro_value_t *value);
        static std::string type_name(avro_value_t *value);

        static void set_null(avro_value_t *value);

        static void set_int(avro_value_t *value,
                            int intvalue);
        static void set_long(avro_value_t *value,
                             long longvalue);
        static void set_float(avro_value_t *value,
                              float floatvalue);
        static void set_double(avro_value_t *value,
                               double doublevalue);
        static void set_boolean(avro_value_t *value,
                                bool boolvalue);
        static void set_enum(avro_value_t *value,
                             int enumvalue);
        static void set_string(avro_value_t *value,
                               const std::string &string);
        static void set_bytes(avro_value_t *value,
                              const core::types::Bytes &bytes);
        static void set_bytes(avro_value_t *value,
                              const std::string &bytes);
        static void set_fixed(avro_value_t *value,
                              const core::types::Bytes &bytes,
                              const std::optional<std::size_t> &nbytes = {});

    protected:
        avro_value_t value;
    };
}  // namespace avro
