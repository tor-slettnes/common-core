// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-publisher.c++
/// @brief Publish demo signals over ZeroMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-publisher.h++"
#include "demo-signals.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace cc::demo::zmq
{
    Publisher::Publisher(const std::string &bind_address,
                         const std::string &channel_name)
        : Super(TYPE_NAME_FULL(This), bind_address, channel_name)
    {
    }

    void Publisher::initialize()
    {
        Super::initialize();

        using namespace std::placeholders;

        // Connect signals from `demo/signals.hpp` to our callback
        // functions.

        // Invoke `on_time_update` whenever there is a time update.
        // This signal is based on the `cc::signal::Signal<>` template,
        // so the callback function will receive one argument: the payload.
        signal_time.connect(
            this->class_name(),
            std::bind(&Publisher::on_time_update, this, _1));

        // Invoke `on_greeting_update` whenever someone sends a greeting.
        // This signal is based on `cc::signal::MappedSignal<>`, so
        // the callback function will receive three arguments:
        //   - The change type (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
        //   - The key (in this case we use the greeter's identity)
        //   - The payload.
        signal_greeting.connect(
            this->class_name(),
            std::bind(&Publisher::on_greeting_update, this, _1, _2, _3));
    }

    void Publisher::deinitialize()
    {
        // We are about to be destroyed, so the callback pointers
        // will no longer remain valid. Disconnect from active signals.
        signal_greeting.disconnect(this->class_name());
        signal_time.disconnect(this->class_name());

        Super::deinitialize();
    }

    void Publisher::on_time_update(const TimeData &time_data)
    {
        logf_trace("Received time update; publishing over ZeroMQ: %s", time_data);
        CC::Demo::Signal msg;
        protobuf::encode(time_data, msg.mutable_time());
        this->publish(msg);
    }

    void Publisher::on_greeting_update(cc::signal::MappingChange change,
                                       const std::string &identity,
                                       const Greeting &greeting)
    {
        logf_trace("Received greeting %s from %r; publishing over ZeroMQ: %s",
                   change,
                   identity,
                   greeting);

        CC::Demo::Signal msg;
        msg.set_change(static_cast<CC::Signal::MappingChange>(change));
        msg.set_key(identity);
        protobuf::encode(greeting, msg.mutable_greeting());
        this->publish(msg);
    }

}  // namespace cc::demo::zmq
