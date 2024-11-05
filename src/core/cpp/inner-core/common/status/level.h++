/// -*- c++ -*-
//==============================================================================
/// @file level.h++
/// @brief Severity level for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <iostream>
#include "types/symbolmap.h++"

namespace core::status
{
    //==========================================================================
    // @enum Level
    // @brief Event level.
    ///
    // @note Keep in sync with `Domain` in `event_types.idl`.  We declare
    //       it again here to avoid dependency on 3rd-party code generation
    //       in core library.

    enum class Level
    {
        NONE,      // No event
        TRACE,     // Trace event for detailed troubleshooting
        DEBUG,     // Debug event for normal troubleshooting
        INFO,      // Informational event only, no error
        NOTICE,    // Important event
        WARNING,   // Abnormal event, operation continues
        FAILED,    // Operation failed, entity is still functional
        CRITICAL,  // Operation failed, entity is disabled
        FATAL      // Reporting entity is disabled and cannot recover
    };

    using LevelMap = types::SymbolMap<
        Level,
        std::multimap<Level, std::string>>;

    extern const LevelMap level_names;

    std::ostream &operator<<(std::ostream &stream, Level lvl);
    std::istream &operator>>(std::istream &stream, Level &lvl);
}  // namespace core::status
