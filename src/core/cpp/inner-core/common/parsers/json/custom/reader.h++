/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "tokenparser.h++"
#include "parsers/common/basereader.h++"

namespace core::json
{
    class CustomReader : public parsers::BaseReader
    {
        using This = CustomReader;
        using Super = parsers::BaseReader;

    public:
        CustomReader();

    public:
        types::Value decoded(const std::string_view &text) const override;
        types::Value read_stream(std::istream &stream) const override;
        using Super::read_stream;

    private:
        static types::Value parse_input(const parsers::Input::ptr &input);
        static types::Value parse_value(TokenParser *parser);
        static types::KeyValueMapPtr parse_object(TokenParser *parser);
        static types::ValueListPtr parse_array(TokenParser *parser);
        static TokenParser::TokenPair next_value(
            TokenParser *parser,
            const TokenParser::TokenMask &endtokens = TokenParser::TI_NONE);
    };
}  // namespace core::json
