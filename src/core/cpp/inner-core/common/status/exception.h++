/// -*- c++ -*-
//==============================================================================
/// @file exception.h++
/// @brief Specializations of some standard C++ exception classes.
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event.h++"
#include "string/format.h++"
#include "types/value.h++"
#include <iostream>

/// Throw exception with formatted message
///
/// Sample usage:
/// ```
///    throwf(exception::Unavailable,                    // Exception type
///           "Scope %r is currently in use",            // Format string
///           scope);                                    // Arguments for str::format()
/// ```

#define throwf(_EXC, ...) throw _EXC(shared::str::format(__VA_ARGS__))

/// Throw exception with formatted message and arguments
///
/// Sample usage:
/// ```
///    throwf_args(
///        exception::Unavailable,                  // Exception type
///        ("Scope %r is currently in use", scope), // Arguments for str::format()
///        scope);                                  // Additional exception arguments
/// ```

#define throwf_args(_EXC, ...) throw _EXC(shared::str::format __VA_ARGS__)

namespace shared::exception
{
    //==========================================================================
    /// @class Exception<E>
    /// @brief Event wrapper for local errors derived on std::exception

    template <class E>
    class Exception : public status::Event,
                      public E
    {
        /// Override the otherwise ambiguous "<<" operator, which is defined for
        /// both the Event and std::exception base classes
        friend inline std::ostream &operator<<(std::ostream &stream,
                                               const Exception<E> &e)
        {
            e.to_stream(stream);
            return stream;
        }

        using Super = status::Event;

    public:
        inline Exception(const status::Event &event)
            : status::Event(event),
              E(event.text())
        {
        }

        inline Exception(const status::Event &event, const E &exc)
            : status::Event(event),
              E(exc)
        {
        }

    protected:
        inline std::string class_name() const noexcept override
        {
            return "Exception";
        }
    };

}  // namespace shared::exception
