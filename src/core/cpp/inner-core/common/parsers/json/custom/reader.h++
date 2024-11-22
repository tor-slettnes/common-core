/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tokenparser-json.h++"
#include "parsers/common/basereader.h++"

namespace core::json
{
    class CustomReader : public parsers::BaseReader
    {
        using This = CustomReader;
        using Super = parsers::BaseReader;

        using TokenParser = parsers::TokenParser;

    public:
        CustomReader();

        types::Value decoded(const std::string_view &string) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        types::Value read_stream(std::istream &&stream) const override;

    private:
        static types::Value parse_input(const parsers::Input::ptr &input);
        static types::Value parse_value(JsonParser *parser);
        static types::KeyValueMapPtr parse_object(JsonParser *parser);
        static types::ValueListPtr parse_array(JsonParser *parser);
        static TokenParser::TokenPair next_value(
            JsonParser *parser,
            const TokenParser::TokenMask &endtokens = TokenParser::TI_NONE);
    };
}  // namespace core::json
