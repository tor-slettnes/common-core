// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"

namespace pubsub
{
    Options::Options()
        : Super(),
          enable_grpc(false),
          enable_zmq(false)
    {
        this->describe("Message Relay.");
    }

    void Options::add_options()
    {
        Super::add_options();

#if USE_GRPC
        this->add_flag(
            {"--grpc", "--enable-grpc"},
            "Enable gRPC service",
            &this->enable_grpc,
            core::settings->get("enable grpc", true).as_bool());
#endif

#if USE_ZMQ
        this->add_flag(
            {"--enable-zmq"},
            "Enable ZeroMQ listener and broadcaster",
            &this->enable_zmq,
            core::settings->get("enable zmq", true).as_bool());

        this->add_opt(
            {"--zmq-producer-interface"},
            "ADDRESS",
            "Bind to specific interface address to listen for incoming ZMQ publications",
            &this->zmq_producer_interface);

        this->add_opt(
            {"--zmq-consumer-interface"},
            "ADDRESS",
            "Bind to specific interface addresss to broadcast outgoing ZMQ publications",
            &this->zmq_consumer_interface);
#endif
    }

    std::unique_ptr<Options> options;
}  // namespace pubsub
