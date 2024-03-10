/// -*- c++ -*-
//==============================================================================
/// @file jsonparser.c++
/// @brief Parse JSON stream as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonparser.h++"
#include "tokenparser-string.h++"
#include "tokenparser-stream.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <stdlib.h>

namespace core::json
{
    types::Value JsonParser::parse_text(const std::string &text)
    {
        logf_debug("Parsing JSON string (%d bytes)", text.size());
        return This::parse_input(std::make_shared<StringParser>(text));
    }

    types::Value JsonParser::parse_stream(std::istream &&stream)
    {
        log_debug("Parsing JSON stream");
        return This::parse_input(std::make_shared<StreamParser>(stream));
    }

    types::Value JsonParser::parse_stream(std::istream &stream)
    {
        log_debug("Parsing JSON stream");
        return This::parse_input(std::make_shared<StreamParser>(stream));
    }

    types::Value JsonParser::parse_input(const ParserRef &parser)
    {
        types::Value value = This::parse_value(parser);
        parser->next_of(TI_END);
        return value;
    }

    types::Value JsonParser::parse_value(const ParserRef &parser)
    {
        return This::next_value(parser).second;
    }

    types::KeyValueMapRef JsonParser::parse_object(const ParserRef &parser)
    {
        auto map = types::KeyValueMap::create_shared();

        for (TokenPair tp = parser->next_of(TI_STRING, TI_OBJECT_CLOSE);
             tp.first != TI_NONE;
             tp = parser->next_of(TI_STRING))
        {
            parser->next_of(TI_COLON);
            map->insert_or_assign(tp.second.as_string(), This::parse_value(parser));

            // types::Value value = This::parse_value(parser);
            // steady::TimePoint start = steady::Clock::now();
            // map->insert_or_assign(tp.second.as_string(), std::move(value));
            // This::map_time_ += steady::Clock::now() - start;
            // This::object_mappings_++;
            if (!parser->next_of(TI_COMMA, TI_OBJECT_CLOSE).first)
            {
                break;
            }
        }

        return map;
    }

    types::ValueListRef JsonParser::parse_array(const ParserRef &parser)
    {
        auto list = types::ValueList::create_shared();

        for (TokenPair tp = This::next_value(parser, {TI_ARRAY_CLOSE});
             tp.first != TI_NONE;
             tp = This::next_value(parser))
        {
            // steady::TimePoint start = steady::Clock::now();
            list->push_back(std::move(tp.second));
            // This::push_time_ += steady::Clock::now() - start;
            // This::array_insertions_++;
            if (!parser->next_of(TI_COMMA, TI_ARRAY_CLOSE).first)
            {
                break;
            }
        }

        return list;
    }

    TokenPair JsonParser::next_value(const ParserRef &parser,
                                     const TokenMask &endtokens)
    {
        static const std::uint64_t value_mask =
            (TI_OBJECT_OPEN | TI_ARRAY_OPEN |
             TI_NULL | TI_BOOL | TI_NUMERIC | TI_STRING);

        auto tp = parser->next_of(value_mask, endtokens);
        switch (tp.first)
        {
        case TI_OBJECT_OPEN:
            return {tp.first, This::parse_object(parser)};

        case TI_ARRAY_OPEN:
            return {tp.first, This::parse_array(parser)};

        default:
            return tp;
        }
    }

}  // namespace core::json
