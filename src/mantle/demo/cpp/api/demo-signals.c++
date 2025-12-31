// -*- c++ -*-
//==============================================================================
/// @file demo-signals.c++
/// @brief Demo App - Internal notification signals
/// @author Tor Slettnes
//==============================================================================

// Application specific modules
#include "demo-signals.h++"

namespace demo
{
    core::signal::DataSignal<TimeData> signal_time("demo::signal_time");
    core::signal::MappingSignal<Greeting> signal_greeting("demo::signal_greeting", true);
}  // namespace demo
