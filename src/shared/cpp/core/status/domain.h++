/// -*- c++ -*-
//==============================================================================
/// @file domain.h++
/// @brief Event domain
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <iostream>
#include "types/symbolmap.h++"

namespace shared::status
{
    //==========================================================================
    // @enum Domain
    // @brief Event domain. Combined with `origin` to identify a code space
    ///
    // @note Keep in sync with `Domain` in `event_types.idl`.  We declare
    //       it again here to avoid dependency on 3rd-party code generation
    //       in core library.

    enum class Domain
    {
        NONE,         // No event
        APPLICATION,  // Event from application, name indicated in "origin"
        SYSTEM,       // OS events, origin indicates code space
        PROCESS,      // Process exit; see `kill -l` to interpret `code`/`symbol`
        PERIPHERAL,   // Event code from a device, name indicated in "origin"
        SERVICE       // Status code from a (network) service (HTTP, SMTP, ...)
    };

    extern const types::SymbolMap<Domain> DomainNames;

    std::ostream &operator<<(std::ostream &stream, Domain domain);
    std::istream &operator>>(std::istream &stream, Domain &domain);

}  // namespace shared::status
