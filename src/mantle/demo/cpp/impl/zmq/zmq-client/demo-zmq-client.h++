// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-client.h++
/// @brief Demo ZeroMQ client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"
#include "demo-zmq-common.h++"
#include "zmq-protobuf-client.h++"

// Shared modules
//#include "zmq-client.h++"
#include "types/create-shared.h++"


namespace demo::zmq
{
    //==========================================================================
    // @class ClientImpl
    // @brief Handle requests from and publish updates to Switchboard clients

    class ClientImpl
        : public API,
          public core::zmq::ProtoBufClient,
          public core::types::enable_create_shared<ClientImpl>
    {
        // Convencience alias
        using This = ClientImpl;
        using Super = API;

    protected:
        ClientImpl(const std::string &identity,
                   const std::string &host_address = "",
                   const std::string &channel_name = SERVICE_CHANNEL,
                   const std::string &interface_name = DEMO_INTERFACE);

    public:
        void initialize() override;
        void deinitialize() override;

        void say_hello(const Greeting &greeting) override;
        TimeData get_current_time() override;
        void start_ticking() override;
        void stop_ticking() override;

        void start_watching() override;
        void stop_watching() override;
    };

}  // namespace demo::zmq
