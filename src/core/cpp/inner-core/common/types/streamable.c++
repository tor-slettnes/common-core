/// -*- c++ -*-
//==============================================================================
/// @file streamable.c++
/// @brief Base to facilitate output sreaming to a data class
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "streamable.h++"

namespace core::types
{
    std::ostream &operator<<(std::ostream &stream, const Streamable &obj)
    {
        obj.to_stream(stream);
        return stream;
    }

    void Streamable::to_literal_stream(std::ostream &stream) const
    {
        this->to_stream(stream);
    }

    std::string Streamable::to_string() const
    {
        std::ostringstream out;
        this->to_stream(out);
        return out.str();
    }

    std::string Streamable::to_literal() const
    {
        std::ostringstream out;
        this->to_literal_stream(out);
        return out.str();
    }

}  // namespace core::types
