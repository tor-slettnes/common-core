/// -*- c++ -*-
//==============================================================================
/// @file jsondecoder.h++
/// @brief Decode rapidjson as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include <iostream>

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}

#include <rapidjson/document.h>

namespace core::json
{
    class JsonDecoder
    {
        using This = JsonDecoder;

    public:
        static types::Value parse_text_with_comments(const std::string &text);
        static types::Value parse_text(std::string &&text);
        // static types::Value parse_stream(std::istream &&stream);
        // static types::Value parse_stream(std::istream &stream);

    protected:
        static std::string uncomment(const std::string &text);
        static types::Value decodeValue(const rapidjson::Value &value);
        static types::ValueListRef decodeArray(const rapidjson::Value &jarray);
        static types::KeyValueMapRef decodeObject(const rapidjson::Value &jobject);
    };
}  // namespace core::json
