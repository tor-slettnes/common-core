/// -*- c++ -*-
//==============================================================================
/// @file avro-base.c++
/// @brief Common functionality wrappers for SimpleValue endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-simplevalue.h++"
#include "avro-status.h++"

namespace avro
{
    SimpleValue::SimpleValue(bool boolean)
        : BaseValue()
    {
        checkstatus(avro_generic_boolean_new(&this->value, boolean));
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
            input.size()));
    }

    SimpleValue::SimpleValue(const std::string_view &input)
        : BaseValue()
    {
        checkstatus(avro_generic_string_new_length(
            &this->value,
            input.data(),
            input.size()));
    }

    SimpleValue::SimpleValue(const core::types::Bytes &bytes)
        : BaseValue()
    {
        checkstatus(avro_generic_bytes_new(
            &this->value,
            (void *)bytes.data(),
            bytes.size()));
    }

    core::types::Value SimpleValue::as_value(bool enums_as_strings) const
    {
        switch (this->avro_type())
        {
        case AVRO_STRING:
        case AVRO_LINK:
            return this->get_string();

        case AVRO_BYTES:
            return this->get_bytes();

        case AVRO_INT32:
            return this->get_int();

        case AVRO_INT64:
            return this->get_long();

        case AVRO_FLOAT:
            return this->get_float();

        case AVRO_DOUBLE:
            return this->get_double();

        case AVRO_BOOLEAN:
            return this->get_boolean();

        case AVRO_NULL:
            return {};

        case AVRO_ENUM:
            if (enums_as_strings)
            {
                return this->get_enum_symbol();
            }
            else
            {
                return this->get_enum_value();
            }

        case AVRO_FIXED:
            return this->get_fixed();

        case AVRO_MAP:
        case AVRO_RECORD:
            return this->get_map(enums_as_strings);

        case AVRO_ARRAY:
            return this->get_array(enums_as_strings);

        case AVRO_UNION:
            return this->get_union(enums_as_strings);

        default:
            return {};
        }
    }

    std::optional<std::string> SimpleValue::get_string() const
    {
        const char *c_string = nullptr;
        std::size_t size;

        if (avro_value_get_string(&this->value, &c_string, &size) == 0)
        {
            return std::string(c_string, size);
        }
        else
        {
            return {};
        }
    }

    std::optional<core::types::ByteVector> SimpleValue::get_bytes() const
    {
        const core::types::Byte *bytes = nullptr;
        std::size_t size = 0;
        if (avro_value_get_bytes(
                &this->value,
                reinterpret_cast<const void **>(&bytes),
                &size) == 0)
        {
            return core::types::ByteVector(bytes, bytes + size);
        }
        else
        {
            return {};
        }
    }

    std::optional<int> SimpleValue::get_int() const
    {
        int result = 0;
        if (avro_value_get_int(&this->value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<long> SimpleValue::get_long() const
    {
        long result = 0;
        if (avro_value_get_long(&this->value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<float> SimpleValue::get_float() const
    {
        float result = 0.0;
        if (avro_value_get_float(&this->value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<double> SimpleValue::get_double() const
    {
        double result = 0.0;
        if (avro_value_get_double(&this->value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<bool> SimpleValue::get_boolean() const
    {
        int result = 0;
        if (avro_value_get_boolean(&this->value, &result))
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<int> SimpleValue::get_enum_value() const
    {
        int result = 0;
        if (avro_value_get_enum(&this->value, &result))
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> SimpleValue::get_enum_symbol() const
    {
        avro_schema_t schema = this->avro_schema();
        int enum_value = 0;

        if ((avro_value_get_enum(&this->value, &enum_value) == 0) &&
            (enum_value < avro_schema_enum_number_of_symbols(schema)))
        {
            return avro_schema_enum_get(schema, enum_value);
        }
        else
        {
            return {};
        }
    }

    std::optional<core::types::ByteVector> SimpleValue::get_fixed() const
    {
        const core::types::Byte *data = nullptr;
        std::size_t size = 0;
        if (avro_value_get_fixed(
                &this->value,
                reinterpret_cast<const void **>(&data),
                &size) == 0)
        {
            return core::types::ByteVector(data, data + size);
        }
        else
        {
            return {};
        }
    }

    core::types::KeyValueMapPtr SimpleValue::get_map(bool enums_as_strings) const
    {
        std::size_t size = 0;
        core::types::KeyValueMapPtr kvmap;
        if (avro_value_get_size(&this->value, &size) == 0)
        {
            kvmap = std::make_shared<core::types::KeyValueMap>();
            for (std::size_t index = 0; index < size; index++)
            {
                const char *key = nullptr;
                avro_value_t avro_value;
                avro_value_get_by_index(
                    &this->value,
                    index,
                    &avro_value,
                    &key);

                if (key != nullptr)
                {
                    kvmap->insert_or_assign(
                        key,
                        SimpleValue(std::move(avro_value)).as_value(enums_as_strings));
                }
            }
        }
        return kvmap;
    }

    core::types::ValueListPtr SimpleValue::get_array(bool enums_as_strings) const
    {
        std::size_t size = 0;
        core::types::ValueListPtr list;
        if (avro_value_get_size(&this->value, &size) == 0)
        {
            list = std::make_shared<core::types::ValueList>(size);

            for (std::size_t index = 0; index < size; index++)
            {
                avro_value_t avro_value;
                checkstatus(
                    avro_value_get_by_index(
                        &this->value,
                        index,
                        &avro_value,
                        nullptr));

                list->at(index) = SimpleValue(std::move(avro_value)).as_value(enums_as_strings);
            }
        }
        return list;
    }

    std::optional<core::types::Value> SimpleValue::get_union(bool enums_as_strings) const
    {
        avro_value_t avro_value;
        if (avro_value_get_current_branch(&this->value, &avro_value) == 0)
        {
            return SimpleValue(std::move(avro_value)).as_value(enums_as_strings);
        }
        else
        {
            return {};
        }
    }

}  // namespace avro
