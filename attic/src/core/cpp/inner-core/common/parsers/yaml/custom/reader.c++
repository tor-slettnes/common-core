/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief Parse YAML content
/// @author Tor Slettnes
//==============================================================================

#include "reader.h++"
#include "parsers/common/parserinput-stream.h++"
#include "parsers/common/parserinput-string.h++"

namespace core::yaml
{
    Reader::Reader()
        : Super("YAML::Reader"),
          indentation(0),
          indentation_stack({indentation})
    {
    }

    types::Value Reader::decoded(const std::string_view &string) const
    {
        return This::parse_input(std::make_shared<parsers::StringInput>(text));
    }

    types::Value Reader::read_file(const fs::path &path) const
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

    types::Value Reader::read_stream(std::istream &&stream) const
    {
        return this->read_stream(stream);
    }

    types::Value Reader::read_stream(std::istream &stream) const
    {
        return This::parse_input(std::make_shared<parsers::StreamInput>(stream));
    }

    types::Value Reader::parse_input(const parsers::Input::ptr &input)
    {
        YamlParser parser(input);
        types::Value value = This::parse_value(&parser);
        parser.next_of(TokenParser::TI_END);
        return value;
    }

    types::Value Reader::parse_value(YamlParser *parser)
    {
        return This::next_value(parser).second;
    }

    types::KeyValueMapPtr Reader::parse_document(YamlParser *parser)
    {
        auto map = types::KeyValueMap::create_shared();
        TokenParser::TokenPair tp = parser->next_of(
            TokenParser::TI_INTRO,
            TokenParser::TI_INDENTATION);

        while (tp.first != TokenParser::TI_NONE)
        {
            tp = parser->next_of(TokenParser::TI_INDENTATION,
                                 TokenParser::TI_SYMBOL);
        }




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

    types::ValueListPtr Reader::parse_array(YamlParser *parser)
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

    parsers::TokenParser::TokenPair Reader::next_value(
        YamlParser *parser,
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

    void Reader::update_indendation(uint indentation)
    {
        this->indentation = indentation;

        if (indentation > this->indentation_stack.back())
        {
            this->indentation_stack.push_back(indentation);
        }
        else
        {
            while (indentation < this->indentation_stack.back())
            {
                this->indentation_stack.pop_back();
            }
            if (indentation...)
        }
    }

}  // namespace core::yaml
