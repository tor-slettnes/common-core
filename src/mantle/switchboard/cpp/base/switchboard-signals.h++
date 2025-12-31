// -*- c++ -*-
//==============================================================================
/// @file switchboard-signals.h++
/// @brief Switchboard signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switch.h++"
#include "thread/signaltemplate.h++"

namespace switchboard
{
    //==========================================================================
    // Telemetry

    void start_event_capture();
    void stop_event_capture();

    //==========================================================================
    // Signals

    extern core::signal::MappingSignal<Specification> signal_spec;
    extern core::signal::MappingSignal<Status> signal_status;

}  // namespace switchboard
