/// -*- c++ -*-
//==============================================================================
/// @file flow.h++
/// @brief Event flow
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <iostream>
#include "types/symbolmap.h++"

namespace shared::status
{
    //==========================================================================
    // @enum Flow
    // @brief Event flow. Combined with `origin` to identify a code space
    ///
    // @note Keep in sync with `Flow` in `event_types.idl`.  We declare
    //       it again here to avoid dependency on 3rd-party code generation
    //       in core library.

    enum class Flow
    {
        NONE,       // No interruption
        PAUSED,     // Execution suspended; may resume.
        CANCELLED,  // Cancelled; state unchanged
        ABORTED,    // Aborted underway; state may have changed
    };

    extern const types::SymbolMap<Flow> FlowNames;

    std::ostream &operator<<(std::ostream &stream, Flow flow);
    std::istream &operator>>(std::istream &stream, Flow &flow);

}  // namespace shared::status
