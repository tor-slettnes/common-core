/// -*- c++ -*-
//==============================================================================
/// @file exception.h++
/// @brief Specializations of some standard C++ exception classes.
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "error.h++"
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

#define throwf(_EXC, ...) throw _EXC(cc::str::format(__VA_ARGS__))

/// Throw exception with formatted message and arguments
///
/// Sample usage:
/// ```
///    throwf_args(
///        exception::Unavailable,                  // Exception type
///        ("Scope %r is currently in use", scope), // Arguments for str::format()
///        scope);                                  // Additional exception arguments
/// ```

#define throwf_args(_EXC, ...) throw _EXC(cc::str::format __VA_ARGS__)

namespace cc::exception
{
    //==========================================================================
    /// @class Exception<E>
    /// @brief status::Error wrapper for local errors derived on std::exception

    template <class E>
    class Exception : public status::Error,
                      public E
    {
        /// Override the otherwise ambiguous "<<" operator, which is defined for
        /// both the Error and std::exception base classes
        friend inline std::ostream &operator<<(std::ostream &stream,
                                               const Exception<E> &e)
        {
            e.to_stream(stream);
            return stream;
        }

        using Super = status::Error;

    public:
        inline Exception(const status::Error &error)
            : status::Error(error),
              E(error.text())
        {
        }

        inline Exception(const status::Error &error, const E &exc)
            : status::Error(error),
              E(exc)
        {
        }
    };

}  // namespace cc::exception
