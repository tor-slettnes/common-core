/// -*- c++ -*-
//==============================================================================
/// @file parserinput-stream.c++
/// @brief Read tokens from stream inputs
/// @author Tor Slettnes
//==============================================================================

#include "parserinput-stream.h++"
#include <sstream>

namespace core::parsers
{
    StreamInput::StreamInput(std::istream &stream)
        : stream_(stream),
          token_position_(0),
          token_size_(0)
    {
        // Enough to contain most miscellaneous content;
        // expanded by powers of two when parsing longer strings.
        this->token_.reserve(64);
    }

    std::size_t StreamInput::token_position() const
    {
        return this->token_position_;
    }

    std::string_view StreamInput::token() const
    {
        return this->token_;
    }

    int StreamInput::getc()
    {
        return this->stream_.get();
    }

    void StreamInput::ungetc(int c)
    {
        this->stream_.unget();
    }

    void StreamInput::init_token()
    {
        this->token_.clear();
        this->token_position_ = this->stream_.tellg();
    }

    void StreamInput::init_token(char c)
    {
        this->token_ = c;
        this->token_position_ = this->stream_.tellg() - std::basic_ios<char>::pos_type(1);
    }

    void StreamInput::append_to_token(char c)
    {
        this->token_.push_back(c);
    }

}  // namespace core::parser
