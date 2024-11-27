/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Read JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parsers/common/basereader.h++"

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}

#include <rapidjson/document.h>

namespace core::json
{
    class RapidReader : public parsers::BaseReader
    {
        using This = RapidReader;
        using Super = parsers::BaseReader;

    public:
        RapidReader();

        types::Value decoded(const std::string_view &text) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        using Super::read_stream;

    private:
        static std::string read_text_from_file(const fs::path &path);
        // static std::string uncomment(const std::string &text);
        static types::Value decode_value(const ::rapidjson::Value &value);
        static types::ValueListPtr decode_array(const ::rapidjson::Value &jarray);
        static types::KeyValueMapPtr decode_object(const ::rapidjson::Value &jobject);
    };
}  // namespace core::json
