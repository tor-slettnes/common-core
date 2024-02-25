/// -*- c++ -*-
//==============================================================================
/// @file level.c++
/// @brief Severity level for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "level.h++"

namespace shared::status
{
    const types::SymbolMap<Level> LevelNames = {
        {Level::NONE, "NONE"},
        {Level::TRACE, "TRACE"},
        {Level::DEBUG, "DEBUG"},
        {Level::INFO, "INFO"},
        {Level::NOTICE, "NOTICE"},
        {Level::WARNING, "WARNING"},
        {Level::FAILED, "FAILED"},
        {Level::CRITICAL, "CRITICAL"},
        {Level::FATAL, "FATAL"},
    };

    std::ostream &operator<<(std::ostream &stream, Level lvl)
    {
        try
        {
            stream << LevelNames.at(lvl);
        }
        catch (const std::out_of_range &)
        {
            stream << "UNKNOWN_" << static_cast<uint>(lvl);
        }
        return stream;
    }

    std::istream &operator>>(std::istream &stream, Level &lvl)
    {
        return LevelNames.from_stream(stream, &lvl);
    }

}  // namespace shared::status
