/// -*- c++ -*-
//==============================================================================
/// @file level.c++
/// @brief Severity level for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "level.h++"

#include <string>

namespace core::status
{
    const LevelMap level_names = {
        {Level::NONE, "NONE"},
        {Level::TRACE, "TRACE"},
        {Level::DEBUG, "DEBUG"},
        {Level::INFO, "INFO"},
        {Level::NOTICE, "NOTICE"},
        {Level::WARNING, "WARNING"},
        {Level::ERROR, "ERROR"},
        {Level::ERROR, "FAILED"},
        {Level::CRITICAL, "CRITICAL"},
        {Level::FATAL, "FATAL"},
    };

    std::ostream &operator<<(std::ostream &stream, Level lvl)
    {
        return level_names.to_stream(stream,
                                     lvl,
                                     "UNKNOWN_" + std::to_string(static_cast<uint>(lvl)));
    }

    std::istream &operator>>(std::istream &stream, Level &lvl)
    {
        return level_names.from_stream(stream, &lvl);
    }

}  // namespace core::status
