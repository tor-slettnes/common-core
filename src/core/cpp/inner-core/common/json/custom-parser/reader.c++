/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "reader.h++"
#include "tokenparser-string.h++"
#include "tokenparser-stream.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <stdlib.h>

#include <fstream>

namespace core::json
{
    CustomReader::CustomReader()
        : Super("CustomReader")
    {
    }

    types::Value CustomReader::decoded(const std::string &text) const
    {
        return This::parse_input(std::make_shared<StringParser>(text));
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
        return This::parse_input(std::make_shared<StreamParser>(stream));
    }

    types::Value CustomReader::read_stream(std::istream &&stream) const
    {
        return this->read_stream(stream);
    }

    types::Value CustomReader::parse_input(const ParserRef &parser)
    {
        types::Value value = This::parse_value(parser);
        parser->next_of(TI_END);
        return value;
    }

    types::Value CustomReader::parse_value(const ParserRef &parser)
    {
        return This::next_value(parser).second;
    }

    types::KeyValueMapRef CustomReader::parse_object(const ParserRef &parser)
    {
        auto map = types::KeyValueMap::create_shared();

        for (TokenPair tp = parser->next_of(TI_STRING, TI_OBJECT_CLOSE);
             tp.first != TI_NONE;
             tp = parser->next_of(TI_STRING))
        {
            parser->next_of(TI_COLON);
            map->insert_or_assign(tp.second.as_string(), This::parse_value(parser));

            if (!parser->next_of(TI_COMMA, TI_OBJECT_CLOSE).first)
            {
                break;
            }
        }

        return map;
    }

    types::ValueListRef CustomReader::parse_array(const ParserRef &parser)
    {
        auto list = types::ValueList::create_shared();

        for (TokenPair tp = This::next_value(parser, {TI_ARRAY_CLOSE});
             tp.first != TI_NONE;
             tp = This::next_value(parser))
        {
            list->push_back(std::move(tp.second));
            if (!parser->next_of(TI_COMMA, TI_ARRAY_CLOSE).first)
            {
                break;
            }
        }

        return list;
    }

    TokenPair CustomReader::next_value(const ParserRef &parser,
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
