/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Read JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "json/basereader.h++"

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}

#include <rapidjson/document.h>

namespace cc::json
{
    class RapidReader : public BaseReader
    {
        using This = RapidReader;
        using Super = BaseReader;

    public:
        RapidReader();

        types::Value decoded_with_comments(const std::string &string) const;
        types::Value decoded(const std::string &string) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        types::Value read_stream(std::istream &&stream) const override;

    private:
        static std::string read_text_from_file(const fs::path &path);
        static std::string uncomment(const std::string &text);
        static types::Value decodeValue(const ::rapidjson::Value &value);
        static types::ValueListRef decodeArray(const ::rapidjson::Value &jarray);
        static types::KeyValueMapRef decodeObject(const ::rapidjson::Value &jobject);
    };
}  // namespace cc::json
