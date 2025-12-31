/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.h++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/streamable.h++"

#include <avro.h>

#include <memory>
// #include <stdexcept>

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

        bool operator==(const BaseValue &other) const;
        bool operator==(const avro_value_t &other) const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        // Public instance methods, accessing the managed value.

        avro_value_t *c_value();
        const avro_value_t *c_value() const;

        avro_value_t &avro_value();
        const avro_value_t &avro_value() const;

        avro_type_t avro_type() const;
        std::string avro_type_name() const;
        const avro_schema_t &avro_schema() const;

        std::size_t serialized_size() const;
        core::types::ByteVector serialized() const;
        std::shared_ptr<core::types::ByteVector> serialized_ptr() const;
        std::string as_json(bool pretty = false) const;
        core::types::Value as_value(bool enums_as_strings = true) const;

    protected:
        avro_value_t value;
    };
}  // namespace avro
