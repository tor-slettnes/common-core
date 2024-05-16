// -*- c++ -*-
//==============================================================================
/// @file demo-signals.c++
/// @brief Demo App - Internal notification signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-signals.h++"

namespace demo
{
    cc::signal::DataSignal<TimeData> signal_time("demo::signal_time");
    cc::signal::MappingSignal<Greeting> signal_greeting("demo::signal_greeting", true);
}  // namespace demo
