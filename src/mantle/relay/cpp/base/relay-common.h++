// -*- c++ -*-
//==============================================================================
/// @file relay-common.h++
/// @brief Message Relay - common definitions
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "types/value.h++"
#include "logging/logging.h++"

namespace relay
{
    define_log_scope("relay");

    //==========================================================================
    // Signals

    extern core::signal::MappingSignal<core::types::Value> signal_message;

}  // namespace relay
