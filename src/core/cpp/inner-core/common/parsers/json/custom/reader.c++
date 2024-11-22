/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "reader.h++"
#include "parsers/common/parserinput-stream.h++"
#include "parsers/common/parserinput-string.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <stdlib.h>

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

    types::Value CustomReader::read_file(const fs::path &path) const
    {
        if (std::ifstream is{path})
        {
            return this->read_stream(is);
        }
        else
        {
            return {};
        }
    }

    types::Value CustomReader::read_stream(std::istream &stream) const
    {
        return This::parse_input(std::make_shared<parsers::StreamInput>(stream));
    }

    types::Value CustomReader::read_stream(std::istream &&stream) const
    {
        return this->read_stream(stream);
    }

    types::Value CustomReader::parse_input(const parsers::Input::ptr &input)
    {
        JsonParser parser(input);
        types::Value value = This::parse_value(&parser);
        parser.next_of(TokenParser::TI_END);
        return value;
    }

    types::Value CustomReader::parse_value(JsonParser *parser)
    {
        return This::next_value(parser).second;
    }

    types::KeyValueMapPtr CustomReader::parse_object(JsonParser *parser)
    {
        auto map = types::KeyValueMap::create_shared();

        for (TokenParser::TokenPair tp = parser->next_of(TokenParser::TI_STRING, TokenParser::TI_OBJECT_CLOSE);
             tp.first != TokenParser::TI_NONE;
             tp = parser->next_of(TokenParser::TI_STRING))
        {
            parser->next_of(TokenParser::TI_COLON);
            map->insert_or_assign(tp.second.as_string(), This::parse_value(parser));

            if (!parser->next_of(TokenParser::TI_COMMA, TokenParser::TI_OBJECT_CLOSE).first)
            {
                break;
            }
        }

        return map;
    }

    types::ValueListPtr CustomReader::parse_array(JsonParser *parser)
    {
        auto list = types::ValueList::create_shared();

        for (TokenParser::TokenPair tp = This::next_value(parser, {TokenParser::TI_ARRAY_CLOSE});
             tp.first != TokenParser::TI_NONE;
             tp = This::next_value(parser))
        {
            list->push_back(std::move(tp.second));
            if (!parser->next_of(TokenParser::TI_COMMA, TokenParser::TI_ARRAY_CLOSE).first)
            {
                break;
            }
        }

        return list;
    }

    parsers::TokenParser::TokenPair CustomReader::next_value(
        JsonParser *parser,
        const TokenParser::TokenMask &endtokens)
    {
        static const std::uint64_t value_mask =
            (TokenParser::TI_OBJECT_OPEN |
             TokenParser::TI_ARRAY_OPEN |
             TokenParser::TI_NULLVALUE |
             TokenParser::TI_BOOL |
             TokenParser::TI_NUMERIC |
             TokenParser::TI_STRING);

        auto tp = parser->next_of(value_mask, endtokens);
        switch (tp.first)
        {
        case TokenParser::TI_OBJECT_OPEN:
            return {tp.first, This::parse_object(parser)};

        case TokenParser::TI_ARRAY_OPEN:
            return {tp.first, This::parse_array(parser)};

        default:
            return tp;
        }
    }

}  // namespace core::json
