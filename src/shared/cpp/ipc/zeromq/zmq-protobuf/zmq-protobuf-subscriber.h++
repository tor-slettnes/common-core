/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-subscriber.h++
/// @brief ZeroMQ subscriber with methods to handle ProtoBuf payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-subscriber.h++"
#include "protobuf-message.h++"

namespace cc::zmq
{
    class ProtoBufSubscriber : public Subscriber
    {
        using This = ProtoBufSubscriber;
        using Super = Subscriber;

        template <class ProtoType>
        using Callback = std::function<void(const ProtoType &msg)>;

    protected:
        using Super::Super;

        template <class ProtoType>
        inline void subscribe(const Callback<ProtoType> &callback)
        {
            Super::subscribe([](const types::ByteArray &bytes) {
                callback(protobuf::to_message<ProtoType>(bytes));
            });
        }
    };

}  // namespace cc::zmq
