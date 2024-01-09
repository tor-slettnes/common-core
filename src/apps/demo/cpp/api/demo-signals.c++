// -*- c++ -*-
//==============================================================================
/// @file demo-signals.c++
/// @brief Demo App - Internal notification signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-signals.h++"

namespace cc::demo
{
    cc::signal::Signal<TimeData> signal_time("cc::demo::signal_time");
    cc::signal::MappedSignal<Greeting> signal_greeting("cc::demo::signal_greeting", true);
}  // namespace cc::demo
