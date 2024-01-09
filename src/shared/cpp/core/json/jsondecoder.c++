/// -*- c++ -*-
//==============================================================================
/// @file jsondecoder.c++
/// @brief Decode rapidjson as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsondecoder.h++"

namespace cc::json
{
    types::Value JsonDecoder::decodeValue(const rapidjson::Value &jv)
    {
        switch (jv.GetType())
        {
        case rapidjson::Type::kNullType:
            return {};

        case rapidjson::Type::kFalseType:
            return false;

        case rapidjson::Type::kTrueType:
            return true;

        case rapidjson::Type::kObjectType:
            return This::decodeObject(jv);

        case rapidjson::Type::kArrayType:
            return This::decodeArray(jv);

        case rapidjson::Type::kStringType:
            return std::string(jv.GetString(), jv.GetStringLength());

        case rapidjson::Type::kNumberType:
            if (jv.IsUint64())
            {
                return jv.GetUint64();
            }
            else if (jv.IsInt64())
            {
                return jv.GetInt64();
            }
            else if (jv.IsUint())
            {
                return jv.GetUint();
            }
            else if (jv.IsInt())
            {
                return jv.GetInt();
            }
            else
            {
                return jv.GetDouble();
            }
            break;

        default:
            return {};
        }
    }

    types::ValueList JsonDecoder::decodeArray(const rapidjson::Value &jarray)
    {
        types::ValueList list;
        list.reserve(jarray.Size());
        for (auto it = jarray.Begin(); it != jarray.End(); it++)
        {
            list.push_back(This::decodeValue(*it));
        }
        return list;
    }

    types::KeyValueMap JsonDecoder::decodeObject(const rapidjson::Value &jobject)
    {
        types::KeyValueMap kvmap;
        for (auto &&item : jobject.GetObject())
        {
            kvmap.insert_or_assign(item.name.GetString(),
                                   This::decodeValue(item.value));
        }
        return kvmap;
    }
}  // namespace cc::json
