// -*- c++ -*-
//==============================================================================
/// @file demo-dds-publisher.c++
/// @brief Publish demo signals over DDS
/// @author Tor Slettnes
//==============================================================================

#include "demo-dds-publisher.h++"
#include "demo-signals.h++"
#include "translate-idl-demo.h++"
#include "translate-idl-inline.h++"
#include "logging/logging.h++"

namespace demo::dds
{
    Publisher::Publisher(const std::string &channel_name, int domain_id)
        : Super(channel_name, domain_id),
          time_writer(this->create_writer<CC::Demo::TimeData>(
              CC::Demo::TIMEDATA_TOPIC,
              false,    // reliable
              false)),  // sync_latest
          greeting_writer(this->create_writer<CC::Demo::Greeting>(
              CC::Demo::GREETING_TOPIC,
              true,   // reliable
              true))  // sync_latest
    {
    }

    void Publisher::initialize()
    {
        Super::initialize();

        using namespace std::placeholders;

        // Connect signals from `demo/signals.hpp` to our callback
        // functions.

        // Invoke `on_time_update` whenever there is a time update.
        // This signal is based on the `core::signal::DataSignal<>` template,
        // so the callback function will receive one argument: the payload.
        signal_time.connect(
            this->to_string(),
            std::bind(&Publisher::on_time_update, this, _1));

        // Invoke `on_greeting_update` whenever someone sends a greeting.
        // This signal is based on `core::signal::MappingSignal<>`, so
        // the callback function will receive three arguments:
        //   - The mapping action (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
        //   - The mapping key (in this case we use the greeter's identity)
        //   - The payload.
        signal_greeting.connect(
            this->to_string(),
            std::bind(&Publisher::on_greeting_update, this, _1, _2, _3));
    }

    void Publisher::deinitialize()
    {
        // We are about to be destroyed, so the callback pointers
        // will no longer remain valid. Disconnect from active signals.
        signal_greeting.disconnect(TYPE_NAME_FULL(This));
        signal_time.disconnect(TYPE_NAME_FULL(This));
    }

    void Publisher::on_time_update(const TimeData &time_data)
    {
        logf_trace("Received time update; publishing over DDS: %s", time_data);
        auto encoded_time = idl::encoded<CC::Demo::TimeData>(time_data);
        this->publish(this->time_writer, encoded_time);
    }

    void Publisher::on_greeting_update(core::signal::MappingAction mapping_action,
                                       const std::string &identity,
                                       const Greeting &greeting)
    {
        logf_trace("Received greeting %s from %r; publishing over DDS: %s",
                   mapping_action,
                   identity,
                   greeting);
        auto encoded_greeting = idl::encoded<CC::Demo::Greeting>(greeting);
        this->publish_change(this->greeting_writer, mapping_action, encoded_greeting);
    }

}  // namespace demo::dds
