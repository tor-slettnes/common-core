/// -*- c++ -*-
//==============================================================================
/// @file avro-basevalue.c++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-basevalue.h++"
#include "avro-status.h++"
#include "string/misc.h++"

#include <stdlib.h>

#include <algorithm>
#include <type_traits>

namespace avro
{
    BaseValue::BaseValue(const avro_value_t &avro_value)
    {
        checkstatus(avro_value_copy(&this->value, &avro_value));
    }

    BaseValue::BaseValue(avro_value_t &&avro_value)
    {
        std::swap(this->value, avro_value);
    }

    BaseValue::BaseValue(const BaseValue &other)
    {
        checkstatus(avro_value_copy(&this->value, &other.value));
    }

    BaseValue::BaseValue(BaseValue &&other)
    {
        std::swap(this->value, other.value);
    }

    BaseValue::~BaseValue()
    {
        avro_value_decref(&this->value);
    }

    BaseValue &BaseValue::operator=(const BaseValue &other)
    {
        avro_value_decref(&this->value);
        checkstatus(avro_value_copy(&this->value, &other.value));
        return *this;
    }

    BaseValue &BaseValue::operator=(BaseValue &&other)
    {
        std::swap(this->value, other.value);
        return *this;
    }

    avro_value_t *BaseValue::c_value()
    {
        return &this->value;
    }

    const avro_value_t *BaseValue::c_value() const
    {
        return &this->value;
    }

    avro_type_t BaseValue::avro_type() const
    {
        return avro_value_get_type(&this->value);
    }

    avro_schema_t BaseValue::avro_schema() const
    {
        return avro_value_get_schema(&this->value);
    }

    core::types::Value BaseValue::as_value(bool enums_as_strings) const
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

    std::string BaseValue::as_json(bool pretty) const
    {
        char *json_str = nullptr;
        checkstatus(avro_value_to_json(&this->value, !pretty, &json_str));
        std::string result(json_str);
        free(json_str);
        return result;
    }

    std::optional<std::string> BaseValue::get_string() const
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

    std::optional<core::types::ByteVector> BaseValue::get_bytes() const
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

    std::optional<int> BaseValue::get_int() const
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

    std::optional<long> BaseValue::get_long() const
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

    std::optional<float> BaseValue::get_float() const
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

    std::optional<double> BaseValue::get_double() const
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

    std::optional<bool> BaseValue::get_boolean() const
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

    std::optional<int> BaseValue::get_enum_value() const
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

    std::optional<std::string> BaseValue::get_enum_symbol() const
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

    std::optional<core::types::ByteVector> BaseValue::get_fixed() const
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

    core::types::KeyValueMapPtr BaseValue::get_map(bool enums_as_strings) const
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
                        BaseValue(std::move(avro_value)).as_value(enums_as_strings));
                }
            }
        }
        return kvmap;
    }

    core::types::ValueListPtr BaseValue::get_array(bool enums_as_strings) const
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

                list->at(index) = BaseValue(std::move(avro_value)).as_value(enums_as_strings);
            }
        }
        return list;
    }

    std::optional<core::types::Value> BaseValue::get_union(bool enums_as_strings) const
    {
        avro_value_t avro_value;
        if (avro_value_get_current_branch(&this->value, &avro_value) == 0)
        {
            return BaseValue(std::move(avro_value)).as_value(enums_as_strings);
        }
        else
        {
            return {};
        }
    }

    avro_type_t BaseValue::type(avro_value_t *value)
    {
        return avro_value_get_type(value);
    }

    avro_schema_t BaseValue::schema(avro_value_t *value)
    {
        return avro_value_get_schema(value);
    }

    std::string BaseValue::type_name(avro_value_t *value)
    {
        return avro_schema_type_name(This::schema(value));
    }

    void BaseValue::set_null(avro_value_t *value)
    {
        checkstatus(avro_value_set_null(value));
    }

    void BaseValue::set_int(avro_value_t *value,
                            int intvalue)
    {
        checkstatus(avro_value_set_int(value, intvalue));
    }

    void BaseValue::set_long(avro_value_t *value,
                             long longvalue)
    {
        checkstatus(avro_value_set_long(value, longvalue));
    }

    void BaseValue::set_float(avro_value_t *value,
                              float floatvalue)
    {
        checkstatus(avro_value_set_float(value, floatvalue));
    }

    void BaseValue::set_double(avro_value_t *value,
                               double doublevalue)
    {
        checkstatus(avro_value_set_double(value, doublevalue));
    }

    void BaseValue::set_boolean(avro_value_t *value,
                                bool boolvalue)
    {
        checkstatus(avro_value_set_boolean(value, boolvalue));
    }

    void BaseValue::set_enum(avro_value_t *value,
                             int enumvalue)
    {
        checkstatus(avro_value_set_enum(value, enumvalue));
    }

    void BaseValue::set_string(avro_value_t *value,
                               const std::string &string)
    {
        checkstatus(avro_value_set_string_len(
            value,
            string.data(),
            string.size()));
    }

    void BaseValue::set_bytes(avro_value_t *value,
                              const core::types::Bytes &bytes)
    {
        checkstatus(avro_value_set_bytes(
            value,
            const_cast<core::types::Byte *>(bytes.data()),
            bytes.size()));
    }

    void BaseValue::set_bytes(avro_value_t *value,
                              const std::string &bytes)
    {
        checkstatus(avro_value_set_bytes(
            value,
            const_cast<char *>(bytes.data()),
            bytes.size()));
    }

    void BaseValue::set_fixed(avro_value_t *value,
                              const core::types::Bytes &bytes,
                              const std::optional<std::size_t> &nbytes)
    {
        checkstatus(avro_value_set_fixed(
            value,
            const_cast<core::types::Byte *>(bytes.data()),
            nbytes.value_or(bytes.size())));
    }

}  // namespace avro
