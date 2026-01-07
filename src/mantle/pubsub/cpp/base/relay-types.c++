// -*- c++ -*-
//==============================================================================
/// @file relay-types.h++
/// @brief Relay common definitions
/// @author Tor Slettnes
//==============================================================================

#include "relay-types.h++"

namespace pubsub
{
    //--------------------------------------------------------------------------
    // Signals

    core::signal::MappingSignal<core::types::Value> signal_publication(
        "signal_publication",
        false);

}  // namespace pubsub
