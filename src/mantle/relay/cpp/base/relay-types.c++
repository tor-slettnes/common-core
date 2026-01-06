// -*- c++ -*-
//==============================================================================
/// @file relay-types.h++
/// @brief Relay common definitions
/// @author Tor Slettnes
//==============================================================================

#include "relay-types.h++"

namespace relay
{
    //--------------------------------------------------------------------------
    // Signals

    core::signal::MappingSignal<core::types::Value> signal_message(
        "signal_message",
        false);

}  // namespace relay
