/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.h++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/streamable.h++"

#include <avro.h>

#include <memory>
#include <stdexcept>

namespace avro
{
    class BaseValue : public core::types::Streamable
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
        avro_value_t *c_value();
        const avro_value_t *c_value() const;
        avro_type_t avro_type() const;
        avro_schema_t avro_schema() const;

        std::size_t serialized_size() const;
        core::types::ByteVector serialized() const;
        std::string as_json(bool pretty = false) const;

    protected:
        void to_stream(std::ostream &stream) const override;


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
