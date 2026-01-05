// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-run.h++
/// @brief Message Relay - ZeroMQ listener initialization
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-run.h++"
#include "relay-zmq-common.h++"
#include "relay-zmq-publisher.h++"
#include "relay-zmq-subscriber.h++"
#include "platform/init.h++"
#include "logging/logging.h++"

namespace relay::zmq
{
    constexpr auto SHUTDOWN_SIGNAL_HANDLE = "relay-zmq-listeners";

    void run_zmq_listeners(
        const std::string &producer_interface,
        const std::string &consumer_interface)
    {
        // Instantiate Server to handle incoming requests from client
        auto subscriber = Subscriber::create_shared(
            producer_interface,
            PRODUCER_CHANNEL,
            core::zmq::Endpoint::Role::HOST);

        // Instantiate Publisher to relay asynchronous events over ZeroMQ
        auto publisher = Publisher::create_shared(
            consumer_interface,
            CONSUMER_CHANNEL,
            core::zmq::Endpoint::Role::HOST);

        publisher->initialize();
        logf_notice("%s is ready on %s", *publisher, publisher->address());

        subscriber->initialize();
        logf_notice("%s is ready on %s", *subscriber, subscriber->address());

        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                log_info("ZMQ receiver is shutting down");
                subscriber->stop();
            });

        subscriber->run();

        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);

        logf_notice("%s is shutting down", *subscriber);
        subscriber->deinitialize();

        logf_notice("%s is shutting down", *publisher);
        publisher->deinitialize();
    }
}
