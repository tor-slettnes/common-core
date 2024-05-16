/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-string.c++
/// @brief JSON token iterator for string inputs
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-string.h++"
#include "logging/logging.h++"

namespace cc::json
{
    StringParser::StringParser(const std::string &string)
        : string_(string),
          it_(string.cbegin()),
          end_(string.cend()),
          token_start_(string.cend())
    {
    }

    std::size_t StringParser::token_position() const
    {
        return std::distance(this->string_.begin(), this->token_start_);
    }

    std::string StringParser::token() const
    {
        return {this->token_start_, this->it_};
    }

    std::string::const_iterator StringParser::token_begin() const
    {
        return this->token_start_;
    }

    std::string::const_iterator StringParser::token_end() const
    {
        return this->it_;
    }

    int StringParser::getc()
    {
        if (this->it_ != this->end_)
        {
            return *this->it_++;
        }
        else
        {
            return std::char_traits<char>::eof();
        }
    }

    void StringParser::ungetc(int c)
    {
        if (c != std::char_traits<char>::eof())
        {
            this->it_--;
        }
    }

    void StringParser::init_token(char c)
    {
        this->token_start_ = this->it_ - 1;
    }

}  // namespace cc::json
