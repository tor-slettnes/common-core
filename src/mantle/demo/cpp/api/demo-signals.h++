// -*- c++ -*-
//==============================================================================
/// @file demo-signals.h++
/// @brief Demo App - Internal notification signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
// Application specific modules
#include "demo-types.h++"

// Shared modules
#include "thread/signaltemplate.h++"

namespace demo
{
    // Connectable signals use to send/receive demo data updates.

    /// @brief Signal emitted at periodic time intervals
    /// @sa `demo::start_ticking()` and `demo::stop_ticking()`
    ///
    /// To receive updates as they are emitted on this signal, connect a
    /// callback function (a.k.a., "slot") - for instance a lambda like this:
    ///
    /// ```
    /// demo::signal_time.connect(
    ///     "My Callback Handle",
    ///     [](const TimeData &time_data) {
    ///         std::cout << "Received signal_time: " << time_data << std::endl;
    ///     });
    /// ```
    ///
    /// To later stop receiveing callbacks, disconnect from this signal:
    ///
    /// ```
    /// demo::signal_time.disconnect("My Callback Handle");
    /// ```
    extern core::signal::DataSignal<TimeData> signal_time;

    /// @brief MappingSignal emitted whenever someone issues a greeting.
    /// @sa `demo::say_hello()`.
    ///
    /// The mapping key in this case is the identity of the greeter.
    ///
    /// To receive updates as they are emitted on this signal, connect a
    /// callback function ("slot"), e.g. a lambda like this:
    ///
    /// ```
    /// demo::signal_geeting.connect(
    ///     "My Callback Handle",
    ///     [](core::signal::MappingAction mapping_action,
    ///        const std::string &greeter,
    ///        const Greeting &greeting)
    ///     {
    ///         str::format(std::cout, "Received greeting from %r: %s", greeter, greeting);
    ///     });
    /// ```
    ///
    /// To later stop receiveing callbacks, disconnect from this signal:
    ///
    /// ```
    /// demo::signal_time.disconnect("My Callback Handle");
    /// ```
    ///
    /// The `caching` flag is enabled on this signal (see demo-signals.cpp), so
    /// when first connecting a callback method to this signal it will be
    /// invoked once for each entity that has already issued a greeting
    /// (cfr. "late subscriber" pattern), with the `mapping_action` parameter
    /// set to `core::signal::MAP_ADDITION`. Subsequent invocations will take
    /// place as this signal is emitted, with `mapping_action` being either
    /// `MAP_ADDITION`, `MAP_UPDATE`, or `MAP_REMOVAL`.
    extern core::signal::MappingSignal<Greeting> signal_greeting;
}  // namespace demo
