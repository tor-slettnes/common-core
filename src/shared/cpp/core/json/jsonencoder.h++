/// -*- c++ -*-
//==============================================================================
/// @file jsonencoder.h++
/// @brief Encode variant values as rapidjson objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"

namespace shared::json
{
    /// @brief Encode `types::Value()` & friends to rapidJson writer instances
    /// @tparam[in] WriterType
    ///      The JsonWriter type to use: `Writer` or `PrettyWriter`.

    template <class WriterType>
    class JsonEncoder : protected WriterType
    {
    public:
        bool encodeValue(const types::Value &value, WriterType *writer)
        {
            static bool warned_tv = false;

            switch (value.type())
            {
            case types::ValueType::NONE:
                return writer->Null();

            case types::ValueType::BOOL:
                return writer->Bool(value.as_bool());

            case types::ValueType::CHAR:
                return writer->String(value.get_if<char>(), 1);

            case types::ValueType::STRING:
                return writer->String(value.get<std::string>());

            case types::ValueType::UINT:
                return writer->Uint64(value.as_uint());

            case types::ValueType::SINT:
                return writer->Int64(value.as_sint());

            case types::ValueType::REAL:
            case types::ValueType::TIMEPOINT:
            case types::ValueType::DURATION:
                return writer->Double(value.as_real());

            case types::ValueType::COMPLEX:
                return this->encodeObject(value.as_kvmap(), writer);

            case types::ValueType::BYTEVECTOR:
                return writer->String(value.get<types::ByteVector>().to_base64());

            case types::ValueType::VALUELIST:
                return this->encodeArray(*value.get_valuelist(), writer);

            case types::ValueType::KVMAP:
                return this->encodeObject(*value.get_kvmap(), writer);

            case types::ValueType::TVLIST:
                if (types::TaggedValueListRef tvlist = value.get_tvlist())
                {
                    if (tvlist->mappable())
                    {
                        return this->encodeObject(*tvlist, writer);
                    }
                    else
                    {
                        // if (!warned_tv && tvlist->tagged())
                        // {
                        //     logf_warning(
                        //         "TaggedValueList contains tags, but they are not unique. "
                        //         "Encoding as JSON array, losing tags: %s",
                        //         value);
                        //     warned_tv = true;
                        // }
                        return this->encodeArray(tvlist->values(), writer);
                    }
                }
                else
                {
                    return writer->Null();
                }

            default:
                // logf_warning("%s cannot be converted to JSON: %s",
                //              value.type_name(),
                //              value);
                return writer->Null();
            }
        }

        bool encodeArray(const types::ValueList &list, WriterType *writer)
        {
            writer->StartArray();
            for (const types::Value &v : list)
            {
                this->encodeValue(v, writer);
            }
            return writer->EndArray();
        }

        bool encodeObject(const types::KeyValueMap &kvmap, WriterType *writer)
        {
            writer->StartObject();

            for (const auto &[k, v] : kvmap)
            {
                writer->String(k);
                this->encodeValue(v, writer);
            }
            return writer->EndObject();
        }

        bool encodeObject(const types::TaggedValueList &tvlist, WriterType *writer)
        {
            writer->StartObject();

            for (const auto &[t, v] : tvlist)
            {
                if (t)
                {
                    writer->String(*t);
                    this->encodeValue(v, writer);
                }
            }
            return writer->EndObject();
        }
    };
}  // namespace shared::json
