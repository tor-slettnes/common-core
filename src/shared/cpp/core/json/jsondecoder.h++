/// -*- c++ -*-
//==============================================================================
/// @file jsondecoder.h++
/// @brief Decode rapidjson as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}

#include <rapidjson/document.h>

namespace shared::json
{
    class JsonDecoder
    {
        using This = JsonDecoder;

    public:
        static types::Value decodeValue(const rapidjson::Value &value);
        static types::ValueList decodeArray(const rapidjson::Value &jarray);
        static types::KeyValueMap decodeObject(const rapidjson::Value &jobject);
    };
}  // namespace shared::json
