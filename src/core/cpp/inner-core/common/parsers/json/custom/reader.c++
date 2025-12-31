/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes
//==============================================================================

#include "reader.h++"
#include "parserinput-stream.h++"
#include "parserinput-string.h++"

#include <fstream>

namespace core::json
{
    CustomReader::CustomReader()
        : Super("JSON::CustomReader")
    {
    }

    types::Value CustomReader::decoded(const std::string_view &text) const
    {
        return This::parse_input(std::make_shared<parsers::StringInput>(text));
    }

    types::Value CustomReader::read_stream(std::istream &stream) const
    {
        return This::parse_input(std::make_shared<parsers::StreamInput>(stream));
    }

    types::Value CustomReader::parse_input(const parsers::Input::ptr &input)
    {
        TokenParser parser(input);
        types::Value value = This::parse_value(&parser);
        parser.next_of(TokenParser::TI_END);
        return value;
    }

    types::Value CustomReader::parse_value(TokenParser *parser)
    {
        return This::next_value(parser).second;
    }

    types::KeyValueMapPtr CustomReader::parse_object(TokenParser *parser)
    {
        auto map = types::KeyValueMap::create_shared();
        TokenParser::TokenPair tp = parser->next_of(TokenParser::TI_QUOTED_STRING,
                                                    TokenParser::TI_MAP_CLOSE);

        while (tp.first != TokenParser::TI_NONE)
        {
            parser->next_of(TokenParser::TI_COLON);
            map->insert_or_assign(tp.second.as_string(), This::parse_value(parser));

            if (!parser->next_of(TokenParser::TI_COMMA, TokenParser::TI_MAP_CLOSE).first)
            {
                break;
            }

            tp = parser->next_of(TokenParser::TI_QUOTED_STRING);
        }

        return map;
    }

    types::ValueListPtr CustomReader::parse_array(TokenParser *parser)
    {
        auto list = types::ValueList::create_shared();

        for (TokenParser::TokenPair tp = This::next_value(parser, {TokenParser::TI_LIST_CLOSE});
             tp.first != TokenParser::TI_NONE;
             tp = This::next_value(parser))
        {
            list->push_back(std::move(tp.second));
            if (!parser->next_of(TokenParser::TI_COMMA, TokenParser::TI_LIST_CLOSE).first)
            {
                break;
            }
        }

        return list;
    }

    TokenParser::TokenPair CustomReader::next_value(
        TokenParser *parser,
        const TokenParser::TokenMask &endtokens)
    {
        static const std::uint64_t value_mask =
            (TokenParser::TI_MAP_OPEN |
             TokenParser::TI_LIST_OPEN |
             TokenParser::TI_NULLVALUE |
             TokenParser::TI_BOOL |
             TokenParser::TI_NUMERIC |
             TokenParser::TI_QUOTED_STRING);

        auto tp = parser->next_of(value_mask, endtokens);
        switch (tp.first)
        {
        case TokenParser::TI_MAP_OPEN:
            return {tp.first, This::parse_object(parser)};

        case TokenParser::TI_LIST_OPEN:
            return {tp.first, This::parse_array(parser)};

        default:
            return tp;
        }
    }

}  // namespace core::json
