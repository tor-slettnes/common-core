// -*- c++ -*-
//==============================================================================
/// @file switchboard-signals.c++
/// @brief Switchboard signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-signals.h++"

namespace switchboard
{
    constexpr auto Capture_Handle = "switchboard::SignalCapture";

    void log_spec_update(core::signal::MappingAction change,
                         const SwitchName &sw_name,
                         const Specification &spec)
    {
        logf_debug("%s: %s", sw_name, spec);
    }

    void log_status_update(core::signal::MappingAction change,
                           const SwitchName &sw_name,
                           const Status &status)
    {
        logf_info("%s: %s", sw_name, status);
    }

    void start_event_capture()
    {
        signal_spec.connect(Capture_Handle, log_spec_update);
        signal_status.connect(Capture_Handle, log_status_update);
    }

    void stop_event_capture()
    {
        signal_spec.disconnect(Capture_Handle);
        signal_status.disconnect(Capture_Handle);
    }

    //==========================================================================
    // Signals

    core::signal::MappingSignal<Specification> signal_spec("signal_spec", true);
    core::signal::MappingSignal<Status> signal_status("signal_status", true);

}  // namespace switchboard
