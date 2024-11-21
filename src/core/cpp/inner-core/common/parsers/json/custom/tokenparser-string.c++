/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-string.c++
/// @brief JSON token iterator for string inputs
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-string.h++"
#include "logging/logging.h++"

namespace core::json
{
    StringParser::StringParser(const std::string_view &string)
        : string_(string),
          pos_(0),
          token_pos_(string.size())
    {
    }

    std::size_t StringParser::token_position() const
    {
        return this->token_pos_;
    }

    std::string_view StringParser::token() const
    {
        return {this->string_.data() + this->token_pos_,
                this->pos_ - this->token_pos_};
    }

    int StringParser::getc()
    {
        if (this->pos_ < this->string_.size())
        {
            return this->string_.at(this->pos_++);
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
            this->pos_--;
        }
    }

    void StringParser::init_token(char c)
    {
        this->token_pos_ = this->pos_ - 1;
    }

}  // namespace core::json
