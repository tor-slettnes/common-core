// -*- c++ -*-
//==============================================================================
/// @file logger-api.c++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-api.h++"

namespace logger
{
    API::API(const std::string &identity)
        : identity_(identity)
    {
    }

    const std::string &API::identity() const
    {
        return this->identity_;
    }
}  // namespace logger
