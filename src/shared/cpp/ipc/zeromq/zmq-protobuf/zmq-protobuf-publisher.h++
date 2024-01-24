/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-publisher.h++
/// @brief ZeroMQ publisher with methods to generate ProtoBuf payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-publisher.h++"
#include "protobuf-message.h++"

namespace cc::zmq
{
    class ProtoBufPublisher : public Publisher
    {
        using This = ProtoBufPublisher;
        using Super = Publisher;

    protected:
        using Super::Super;

        using Super::publish;
        void publish(const google::protobuf::Message &msg);
    };

}  // namespace cc::zmq
