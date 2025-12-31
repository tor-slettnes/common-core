/// -*- c++ -*-
//==============================================================================
/// @file streamable.h++
/// @brief Base to facilitate output sreaming to a data class
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <sstream>
#include <string>

namespace core::types
{
    class Streamable
    {
        friend std::ostream &operator<<(std::ostream &stream, const Streamable &obj);

    public:
        /// Send a representation of this object to the output stream.
        /// Implementations must overload this method.
        virtual void to_stream(std::ostream &stream) const = 0;

        /// Send a literal representation of this object to the output stream.
        /// Implementations may overload this to add suitable representation format.
        virtual void to_literal_stream(std::ostream &stream) const;

        /// Convenience wrapper around the above, making use of the same
        /// implementation for plain string conversion.
        virtual std::string to_string() const;
        virtual std::string to_literal() const;
    };

}  // namespace core::types
