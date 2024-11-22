/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-base.h++"
#include "status/exceptions.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::parsers
{
    const int eof = std::char_traits<char>::eof();

    TokenParser::TokenParser(parsers::Input::ptr input,
                             const CommentStyles &comment_styles,
                             const SymbolMapping &symbol_map)
        : input(input),
          comment_styles(comment_styles),
          symbol_map(symbol_map)
    {
    }

    TokenParser::TokenPair TokenParser::next_of(const TokenMask &expected,
                                                const TokenMask &endtokens)
    {
        TokenPair tp = this->next_token();
        while (tp.first == TI_LINE_COMMENT)
        {
            tp = this->next_token();
        }

        if (expected & tp.first)
        {
            return tp;
        }
        else if (endtokens & tp.first)
        {
            return TokenPair(TI_NONE, {});
        }
        else if (tp.first == TI_END)
        {
            throwf(exception::MissingArgument,
                   "Missing token at end of input");
        }
        else if (tp.first == TI_INVALID)
        {
            throwf(exception::InvalidArgument,
                   "Invalid input at position %d: %s",
                   this->input->token_position(),
                   this->input->token());
        }
        else
        {
            throwf(exception::InvalidArgument,
                   "Unexpected token at position %d: %s",
                   this->input->token_position(),
                   this->input->token());
        }
    }

    TokenParser::TokenPair TokenParser::parse_number()
    {
        int c;
        bool got_sign = (*this->input->token().begin() == '-');
        bool got_real = (*this->input->token().begin() == '.');
        bool got_hex = false;

        while (this->token_index(c = this->input->getc()) == TI_NONE)
        {
            this->input->append_to_token(c);
            switch (c)
            {
            case '.':
            case 'e':
            case 'E':
                got_real = true;
                break;

            case 'x':
            case 'X':
                got_hex = true;
                break;
            }
        }
        this->input->ungetc(c);

        types::Value value;
        if (got_real)
        {
            return this->parse_numeric<double>();
        }
        else if (got_sign)
        {
            return this->parse_numeric<std::int64_t>();
        }
        else if (got_hex)
        {
            return this->parse_numeric<std::uint64_t>(0);
        }
        else
        {
            return this->parse_numeric<std::uint64_t>();
        }
    }

    TokenParser::TokenPair TokenParser::parse_symbol()
    {
        int c;
        while (this->token_index(c = this->input->getc()) == TI_NONE)
        {
            this->input->append_to_token(c);
        }
        this->input->ungetc(c);

        try
        {
            return this->symbol_map.at(this->input->token());
        }
        catch (const std::out_of_range &e)
        {
            return {TI_INVALID, {}};
        }
    }

    TokenParser::TokenPair TokenParser::parse_line_comment()
    {
        int c = '\0';
        while ((c = this->input->getc()) != eof)
        {
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
            this->input->append_to_token(c);
        }

        for (const std::string_view &candidate : this->comment_styles)
        {
            if (this->input->token().compare(0, candidate.size(), candidate.data()) == 0)
            {
                return {TI_LINE_COMMENT, {}};
            }
        }
        return {TI_INVALID, {}};
    }

    TokenParser::TokenPair TokenParser::parse_string(char quote, bool raw)
    {
        int c = '\0';
        bool escape = false;
        std::string string;
        std::size_t size = 0;
        std::size_t capacity = 64;
        string.reserve(capacity);

        while ((c = this->input->getc()) != eof)
        {
            this->input->append_to_token(c);

            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\' && !raw)
            {
                escape = true;
                continue;
            }
            else if (c == quote)
            {
                return {TI_STRING, string};
            }

            if (++size >= capacity)
            {
                capacity = std::max(std::size_t(64), 2 * capacity);
                string.reserve(capacity);
            }
            string.push_back(c);
        }

        return {TI_END, string};
    }

    char TokenParser::escape(char c)
    {
        switch (c)
        {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 'f':
            return '\f';
        case 'e':
            return '\x1b';
        default:
            return c;
        }
    }
}  // namespace core::parsers
