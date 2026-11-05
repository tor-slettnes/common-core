/// -*- c++ -*-
//==============================================================================
/// @file exception.h++
/// @brief Specializations of some standard C++ exception classes.
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "error.h++"
#include "string/format.h++"
#include "types/value.h++"
#include <iostream>
#include <stdexcept>
#include <future>

/// Throw exception with formatted message
///
/// Sample usage:
/// ```
///    throwf(exception::Unavailable,                    // Exception type
///           "Scope %r is currently in use",            // Format string
///           scope);                                    // Arguments for str::format()
/// ```

#define throwf(_EXC, ...) throw _EXC(core::str::format(__VA_ARGS__))

/// Throw exception with formatted message and arguments
///
/// Sample usage:
/// ```
///    throwf_args(
///        exception::Unavailable,                  // Exception type
///        ("Scope %r is currently in use", scope), // Arguments for str::format()
///        scope);                                  // Additional exception arguments
/// ```

#define throwf_args(_EXC, ...) throw _EXC(core::str::format __VA_ARGS__)

namespace cc::core::exception
{
    //==========================================================================
    /// @class Exception<E>
    /// @brief Event wrapper for local errors derived on std::exception

    template <class E>
    class Exception : public status::Error,
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

        using Super = status::Error;

    public:
        inline Exception(const status::Error &event,
                         const std::optional<std::string> &name = {})
            : Super(event),
              E(event.text()),
              name_(name)
        {
        }

        inline Exception(const status::Error &event,
                         const E &exc,
                         const std::optional<std::string> &name = {})
            : Super(event),
              E(exc),
              name_(name)
        {
        }

        // template <class ... Args>
        // inline Exception(Args &&... args)
        //     : Super(args...),
        //       E("")
        // {
        // }

    protected:
        inline std::string name() const noexcept override
        {
            if (this->name_)
            {
                return *this->name_;
            }
            else
            {
                return Error::name();
            }
        }

    private:
        const std::optional<std::string> name_;
    };

}  // namespace cc::core::exception
