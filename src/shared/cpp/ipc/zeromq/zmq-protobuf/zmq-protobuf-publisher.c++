/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-publisher.c++
/// @brief ZeroMQ publisher with methods to generate ProtoBuf payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-protobuf-publisher.h++"

namespace cc::zmq
{
    void ProtoBufPublisher::publish(const google::protobuf::Message &msg)
    {
        Super::publish(protobuf::to_bytes(msg));
    }

}  // namespace cc::zmq
