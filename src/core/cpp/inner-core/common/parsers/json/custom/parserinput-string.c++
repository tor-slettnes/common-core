/// -*- c++ -*-
//==============================================================================
/// @file parserinput-string.c++
/// @brief Read tokens from strings
/// @author Tor Slettnes
//==============================================================================

#include "parserinput-string.h++"

namespace core::parsers
{
    StringInput::StringInput(const std::string_view &string)
        : string_(string),
          pos_(0),
          token_pos_(string.size())
    {
    }

    std::size_t StringInput::token_position() const
    {
        return this->token_pos_;
    }

    std::string_view StringInput::token() const
    {
        return {this->string_.data() + this->token_pos_,
                this->pos_ - this->token_pos_};
    }

    int StringInput::getc()
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

    void StringInput::ungetc(int c)
    {
        if (c != std::char_traits<char>::eof())
        {
            this->pos_--;
        }
    }

    void StringInput::init_token()
    {
        this->token_pos_ = this->pos_;
    }

    void StringInput::init_token(char c)
    {
        this->token_pos_ = this->pos_ - 1;
    }

    void StringInput::append_to_token(char c)
    {
    }

}  // namespace core::json
