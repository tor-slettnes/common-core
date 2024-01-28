// -*- c++ -*-
//==============================================================================
/// @file demo-dds-subscriber.c++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-dds-subscriber.h++"
#include "translate-idl-demo.h++"

// Shared modules
#include "translate-idl-inline.h++"

// C++ STL modules
#include <functional>

namespace cc::demo::dds
{
    Subscriber::Subscriber(const std::string &channel_name, int domain_id)
        : Super(channel_name, domain_id),
          time_reader(this->create_reader<CC::Demo::TimeData>(
              CC::Demo::TIMEDATA_TOPIC,    // topic_name
              Subscriber::on_time_update,  // handler
              false,                       // reliable
              false)),                     // sync_latest
          greeting_reader(this->create_reader<CC::Demo::Greeting>(
              CC::Demo::GREETING_TOPIC,        // topic_name
              Subscriber::on_greeting_update,  // handler
              true,                            // reliable
              true))                           // sync_latest
    {
    }

    void Subscriber::on_time_update(cc::signal::MappingChange change,
                                    const CC::Demo::TimeData &time_data)
    {
        // We have received a time update from a remote publisher.
        // Emit this update locally via `cc::demo::signal_time`, declared in
        // `demo-signals.h++'.
        logf_trace("Received time data %s: %s", change, time_data);
        signal_time.emit(idl::decoded<TimeData>(time_data));
    }

    void Subscriber::on_greeting_update(cc::signal::MappingChange change,
                                        const CC::Demo::Greeting &greeting)
    {
        // We have received a time update from a remote publisher.
        // Emit this update locally via `cc::demo::signal_time`, declared
        // in `demo-signals.h++'.
        logf_trace("Received greeting %s: %s", change, greeting);
        signal_greeting.emit(change,
                             greeting.identity(),
                             idl::decoded<Greeting>(greeting));
    }
}  // namespace cc::demo::dds
