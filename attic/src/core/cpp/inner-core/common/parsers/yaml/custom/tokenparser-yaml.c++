/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-yaml.c++
/// @brief Iterate over interesting YAML tokens
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-yaml.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::yaml
{
    const int eof = std::char_traits<char>::eof();

    YamlParser::YamlParser(parsers::Input::ptr input)
        : Super(input),
          line_indent(0),
          indentation_stack({{TI_NONE, 0}})
    {
    }

    parsers::TokenParser::TokenPair YamlParser::next_token()
    {
        while (int c = this->input->getc())
        {
            this->input->init_token(c);
            switch (c)
            {
            case '\r':
            case '\n':
                this->process_newline();
                continue;

            case ' ':
                if (!this->line_indent)
                {
                    return this->parse_indent();
                }
                continue;

            case '-':
                return !this->line_indent ? this->parse_start_document()
                       : !this->key       ? this->parse_list_item()
                                          : this->parse_value();

            case '#':
                return this->parse_line_comment();

            case '"':
            case '\'':
                return this->parse_quoted_string(c);

            case std::char_traits<char>::eof():
                return {TI_EOF, {}};

            default:
                return this->key         ? this->parse_value()
                       : std::isalpha(c) ? this->parse_key()
                                         : {TI_INVALID, this->token()};
            }
        }

        return {TI_NONE, {}};
    }

    parsers::TokenParser::TokenPair YamlParser::next_token()
    {
        while (int c = this->input->getc())
        {
            this->input->init_token(c);
            switch (c)
            {
            case '\r':
            case '\n':
                this->process_newline();
                continue;

            case ' ':
                if (!this->line_indent)
                {
                    return this->parse_indent();
                }
                break;

            case '-':
                return !this->line_indent ? this->parse_start_document()
                       : !this->key       ? this->parse_list_item()
                                          : this->parse_value();

            case '#':
                return this->parse_line_comment();

            case '"':
            case '\'':
                return this->parse_quoted_string(c);

            case std::char_traits<char>::eof():
                return {TI_EOF, {}};

            default:
                return this->key         ? this->parse_value()
                       : std::isalpha(c) ? this->parse_key()
                                         : {TI_INVALID, this->token()};
            }
        }

        return {TI_NONE, {}};
    }

    void YamlParser::process_newline()
    {
        this->line_indent = 0;
    }

    parsers::TokenParser::TokenPair YamlParser::parse_start_document()
    {
        this->input->init_token();
        int c = this->input->getc();
        while (c == '-')
        {
            this->input->append_to_token(c);
            c = this->input->get(c);
        }
        this->input->ungetc(c);

        return {
            this->input()->token().size() == 3 ? TI_START_DOCUMENT : TI_INVALID,
            this->token(),
        };
    }

    parsers::TokenParser::TokenPair YamlParser::parse_indent()
    {
        int c = this->input->token().front();
        while (c == ' ')
        {
            this->input->append_to_token(c);
            c = this->input->getc();
        }
        this->input->ungetc(c);
        std::size_t indent = this->input->token().size();

        return c == '-'          ? this->set_indent(TI_DASH, indent)
               : std::isalpha(c) ? this->set_indent(TI_SYMBOL, indent)
                                 : {TI_SPACE, this->input->token()};
    }

    parsers::TokenParser::TokenPair YamlParser::parse_symbol()
    {
        this->capture_symbol();
    }

    parsers::TokenParser::TokenPair YamlParser::set_indent(TokenIndex ti, std::size_t indent)
    {
        auto [last_type, last_indent] = this->indentation_stack.back();

        if ((indent > last_indent) ||
            ((indent == last_indent) && (ti == TI_DASH) && (last_type == TI_SYMBOL)))
        {
            this->indentation_stack.emplace_back(ti, indent);
            return {(ti == TI_DASH) ? TI_LIST_OPEN : TI_MAP_OPEN,
                    this->indentation_stack.size() - 1};
        }
        else if ((indent < last_indent) ||
                 ((indent == last_indent) && (ti == TI_SYMBOL) && (last_type == TI_DASH)))
        {
            this->indentation_stack.pop_back();
            return {(last_type == TI_DASH) ? TI_LIST_CLOSE : TI_MAP_CLOSE,
                    this->indentation_stack.size() - 1};
        }
        else
        {
            return {TI_SPACE, this->input->token()};
        }
    }

}  // namespace core::yaml
