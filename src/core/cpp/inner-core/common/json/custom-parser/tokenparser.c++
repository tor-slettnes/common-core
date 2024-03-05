/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <charconv>

namespace core::json
{
    TokenParser::TokenParser(const std::string &string)
        : string_(string),
          it_(string.cbegin()),
          end_it_(string.cend()),
          token_start_(string.cbegin()),
          token_end_(string.cbegin()),
          string_parse_time_(dt::Duration::zero()),
          any_parse_time_(dt::Duration::zero())
    {
        logf_debug("TokenParser created");

        // Reserve an initial capacity enough to contain most miscellaneous
        // content.  Expanded by powers of two when parsing longer strings.
        this->token_.reserve(64);
    }

    TokenParser::~TokenParser()
    {
        logf_debug("TokenParser spent %s parsing strings, %s parsing other types",
                   this->string_parse_time_,
                   this->any_parse_time_);
    }

    const std::string &TokenParser::token() const
    {
        return this->token_;
    }

    const types::Value &TokenParser::value() const
    {
        return this->value_;
    }

    TokenIndex TokenParser::next_of(const TokenSet &candidates,
                                    const TokenSet &endtokens)
    {
        TokenIndex index = this->next_token();
        while (index == TI_LINE_COMMENT)
        {
            index = this->next_token();
        }

        if (candidates.count(index))
        {
            return index;
        }
        else if (endtokens.count(index))
        {
            return TI_NONE;
        }
        else if (index == TI_NONE)
        {
            throwf(exception::MissingArgument,
                   "Missing JSON token at end of input");
        }
        else
        {
            throwf(exception::InvalidArgument,
                   "Unexpected JSON token type %d: %r",
                   index,
                   this->token());
        }
    }

    TokenIndex TokenParser::next_token()
    {
        bool got_alpha = false;
        bool got_digit = false;
        bool got_sign = false;
        bool got_hex = false;
        bool got_real = false;
        bool got_string = false;
        bool got_comment = false;

        this->token_.clear();  // Does not change capacity
        this->value_.clear();

        while (this->it_ != this->end_it_)
        {
            char c = *this->it_++;
            TokenIndex ti = TI_NONE;

            switch (c)
            {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case '\v':
            case '\f':
                ti = TI_SPACE;
                break;

            case '{':
                ti = TI_OBJECT_OPEN;
                break;

            case '}':
                ti = TI_OBJECT_CLOSE;
                break;

            case '[':
                ti = TI_ARRAY_OPEN;
                break;

            case ']':
                ti = TI_ARRAY_CLOSE;
                break;

            case ',':
                ti = TI_COMMA;
                break;

            case ':':
                ti = TI_COLON;
                break;

            case '#':
            case '/':
                got_comment = true;
                break;

            case '"':
                got_string = true;
                break;

            case '-':
                got_sign = true;
                break;

            default:
                if (!got_digit && std::isalpha(c))
                {
                    got_alpha = true;
                }
                else if (!got_digit && std::isdigit(c))
                {
                    got_digit = true;
                }
                else if (got_digit && ((c == 'e') || (c == 'E')))
                {
                    got_real = true;
                }
                else if (!got_alpha && (c == '.'))
                {
                    got_real = true;
                }
                else if (got_digit && !got_hex && ((c == 'x') || (c == 'x')))
                {
                    got_hex = true;
                }
                break;
            }

            if (ti)
            {
                if (this->got_token())
                {
                    this->it_--;
                    break;
                }
                else if (ti != TI_SPACE)
                {
                    this->append_to_token(c);
                    return ti;
                }
            }
            else
            {
                this->append_to_token(c);
                if (got_string)
                {
                    return this->parse_string();
                }
                else if (got_comment)
                {
                    return this->parse_line_comment();
                }
            }
        }

        return got_alpha           ? this->parse_symbol()
               : got_digit         ? (got_real   ? this->parse_real()
                                      : got_sign ? this->parse_sint()
                                      : got_hex  ? this->parse_uint(0)
                                                 : this->parse_uint())
               : this->got_token() ? TI_UNKNOWN
                                   : TI_NONE;
    }

    template <class T, class... Args>
    TokenIndex TokenParser::parse_number(TokenIndex ti, const std::string &type, Args &&...args)
    {
        char *const start = this->token_.data();
        char *const end = start + this->token_.size();
        T value = 0;
        std::errc ok;

        auto [ptr, ec] = std::from_chars(start, end, value, args...);
        if ((ec == ok) && (ptr >= end))
        {
            this->value_ = value;
            return ti;
        }
        else
        {
            logf_info("Failed to convert token %r to %s; decoded %d characters: %s",
                      this->token_,
                      type,
                      ptr - start,
                      std::make_error_code(ec).message());
            return TI_UNKNOWN;
        }
    }

    TokenIndex TokenParser::parse_real()
    {
        return this->parse_number<double>(TI_REAL, "real"s);
    }

    TokenIndex TokenParser::parse_sint()
    {
        return this->parse_number<std::int64_t>(TI_SINT, "signed integer"s);
    }

    TokenIndex TokenParser::parse_uint(int base)
    {
        return this->parse_number<std::uint64_t>(TI_UINT, "unsigned integer"s, base);
    }

    TokenIndex TokenParser::parse_symbol()
    {
        static const std::unordered_map<
            std::string,
            std::pair<TokenIndex, types::Value>>
            symbol_map = {
                {"null", {TI_NULL, types::Value()}},
                {"false", {TI_BOOL, types::Value(false)}},
                {"true", {TI_BOOL, types::Value(true)}},
            };

        try
        {
            auto [ti, value] = symbol_map.at(this->token_);
            this->value_ = value;
            return ti;
        }
        catch (const std::out_of_range &e)
        {
            return TI_UNKNOWN;
        }
    }

    TokenIndex TokenParser::parse_line_comment()
    {
        while (this->it_ != this->end_it_)
        {
            char c = *this->it_++;
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
            this->append_to_token(c);
        }

        if (str::startswith(this->token(), "//") ||
            str::startswith(this->token(), "#"))
        {
            return TI_LINE_COMMENT;
        }
        else
        {
            return TI_UNKNOWN;
        }
    }

    TokenIndex TokenParser::parse_string()
    {
        steady::TimePoint start = steady::Clock::now();
        bool escape = false;
        std::string value;

        while (this->it_ != this->end_it_)
        {
            char c = *this->it_++;
            this->append_to_token(c);

            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\')
            {
                escape = true;
                continue;
            }
            else if (c == '"')
            {
                this->string_parse_time_ += (steady::Clock::now() - start);
                this->value_ = value;
                return TI_STRING;
            }

            if (value.size() >= value.capacity())
            {
                value.reserve(std::max(std::size_t(64), 2 * value.capacity()));
            }
            value.push_back(c);
        }

        this->string_parse_time_ += (steady::Clock::now() - start);
        return TI_NONE;
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

    void TokenParser::append_to_token(char c)
    {
        if (this->token_.size() >= this->token_.capacity())
        {
            this->token_.reserve(std::max(std::size_t(16), 2 * this->token_.capacity()));
        }
        this->token_.push_back(c);
    }

    bool TokenParser::got_token() const
    {
        return !this->token_.empty();
    }


}  // namespace core::json
