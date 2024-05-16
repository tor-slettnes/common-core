/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-stream.c++
/// @brief JSON token iterator for stream inputs
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-stream.h++"
#include <sstream>

namespace cc::json
{
    StreamParser::StreamParser(std::istream &stream)
        : stream_(stream),
          token_position_(0),
          token_size_(0),
          token_capacity_(64)
    {
        // Enough to contain most miscellaneous content;
        // expanded by powers of two when parsing longer strings.
        this->token_.reserve(this->token_capacity_);
    }

    std::size_t StreamParser::token_position() const
    {
        return this->token_position_;
    }

    std::string StreamParser::token() const
    {
        return this->token_;
    }

    std::string::const_iterator StreamParser::token_begin() const
    {
        return this->token_.begin();
    }

    std::string::const_iterator StreamParser::token_end() const
    {
        return this->token_.end();
    }

    int StreamParser::getc()
    {
        return this->stream_.get();
    }

    void StreamParser::ungetc(int c)
    {
        this->stream_.unget();
    }

    void StreamParser::init_token(char c)
    {
        this->token_ = c;
        this->token_size_ = 1;
        this->token_capacity_ = this->token_.capacity();
        this->token_position_ = this->stream_.tellg() - std::basic_ios<char>::pos_type(1);
    }

    void StreamParser::append_to_token(char c)
    {
        if (++this->token_size_ > this->token_capacity_)
        {
            this->token_capacity_ = std::max(std::size_t(64), 2 * this->token_capacity_);
            this->token_.reserve(this->token_capacity_);
        }
        this->token_.push_back(c);
    }

}  // namespace cc::json
