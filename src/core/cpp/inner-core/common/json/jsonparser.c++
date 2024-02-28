/// -*- c++ -*-
//==============================================================================
/// @file jsonparser.c++
/// @brief Parse JSON stream as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonparser.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"

namespace core::json
{
    types::Value JsonParser::parse_text(const std::string &text)
    {
        std::shared_ptr<TokenParser> parser = std::make_shared<TokenParser>(text);
        types::Value value = This::parse_value(parser);
        parser->next_of({}, {TI_NONE});
        return value;
    }

    types::Value JsonParser::parse_value(const ParserRef &parser)
    {
        return This::parse_optional(parser, {}).value_or(types::nullvalue);
    }

    types::KeyValueMapRef JsonParser::parse_object(const ParserRef &parser)
    {
        auto map = types::KeyValueMap::create_shared();

        for (TokenIndex token = parser->next_of({TI_STRING}, {TI_OBJECT_CLOSE});
             token;
             token = parser->next_of({TI_STRING}))
        {
            std::string key{parser->token()};
            parser->next_of({TI_COLON});
            map->insert_or_assign(std::move(key), This::parse_value(parser));
            if (!parser->next_of({TI_COMMA}, {TI_OBJECT_CLOSE}))
            {
                break;
            }
        }
        return map;
    }

    types::ValueListRef JsonParser::parse_array(const ParserRef &parser)
    {
        auto list = types::ValueList::create_shared();

        for (std::optional<types::Value> value = This::parse_optional(parser, {TI_ARRAY_CLOSE});
             value;
             value = This::parse_optional(parser, {}))
        {
            list->push_back(*value);
            if (!parser->next_of({TI_COMMA}, {TI_ARRAY_CLOSE}))
            {
                break;
            }
        }
        return list;
    }

    std::optional<types::Value> JsonParser::parse_optional(
        const ParserRef &parser,
        const TokenSet &endtokens)
    {
        while (TokenIndex token = parser->next_of({TI_OBJECT_OPEN,
                                                   TI_ARRAY_OPEN,
                                                   TI_NULL,
                                                   TI_BOOL,
                                                   TI_REAL,
                                                   TI_SINT,
                                                   TI_UINT,
                                                   TI_STRING,
                                                   TI_COMMENT},
                                                  endtokens))
        {
            switch (token)
            {
            case TI_OBJECT_OPEN:
                return This::parse_object(parser);

            case TI_ARRAY_OPEN:
                return This::parse_array(parser);

            case TI_NULL:
                return types::Value();

            case TI_BOOL:
                return parser->token() == "true";

            case TI_REAL:
                return std::stod(parser->token());

            case TI_SINT:
                return std::stoll(parser->token());

            case TI_UINT:
                return std::stoull(parser->token());

            case TI_STRING:
                return str::unescaped(parser->token());

            case TI_COMMENT:
                continue;
            }
        }
        return {};
    }
}  // namespace core::json
