// -*- c++ -*-
//==============================================================================
/// @file relay-common.c++
/// @brief Message Relay - common definitions
/// @author Tor Slettnes
//==============================================================================

#include "relay-common.h++"

namespace relay
{
    //==========================================================================
    // Signals

    core::signal::MappingSignal<core::types::Value> signal_message(
        "signal_message",
        false);

}  // namespace relay
