// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-client.c++
/// @brief Demo ZeroMQ client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application-specific modules
#include "demo-zmq-client.h++"
#include "protobuf-demo-types.h++"      // Demo data encode/decode routines
#include "protobuf-standard-types.h++"  // Encode/decode Google-provided types
#include "protobuf-inline.h++"          // `encoded<>` and `decoded<>` templates
#include "platform/symbols.h++"

namespace cc::demo::zmq
{
    ClientImpl::ClientImpl(const std::string &identity,
                           const std::string &host_address,
                           const std::string &channel_name,
                           const std::string &interface_name)
        : API(identity, "ZMQ ProtoBuf client"),
          ProtoBufClient(host_address, channel_name, interface_name)
    {
    }

    void ClientImpl::initialize()
    {
        API::initialize();
        cc::zmq::ProtoBufClient::initialize();
    }

    void ClientImpl::deinitialize()
    {
        cc::zmq::ProtoBufClient::deinitialize();
        API::deinitialize();
    }

    void ClientImpl::say_hello(const Greeting &greeting)
    {
        this->call(METHOD_SAY_HELLO,
                   protobuf::encoded<CC::Demo::Greeting>(greeting));
    }

    TimeData ClientImpl::get_current_time()
    {
        return protobuf::decoded<TimeData>(
            this->call<CC::Demo::TimeData>(METHOD_GET_CURRENT_TIME));
    }

    void ClientImpl::start_ticking()
    {
        this->call(METHOD_START_TICKING);
    }

    void ClientImpl::stop_ticking()
    {
        this->call(METHOD_STOP_TICKING);
    }

    void ClientImpl::start_watching()
    {
    }

    void ClientImpl::stop_watching()
    {
    }

}  // namespace cc::demo::zmq
