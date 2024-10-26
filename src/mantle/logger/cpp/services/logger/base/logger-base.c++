// -*- c++ -*-
//==============================================================================
/// @file logger-base.c++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-base.h++"

namespace logger
{
    BaseLogger::BaseLogger(const std::string &identity)
        : identity_(identity)
    {
    }

    const std::string &BaseLogger::identity() const
    {
        return this->identity_;
    }
}  // namespace logger
