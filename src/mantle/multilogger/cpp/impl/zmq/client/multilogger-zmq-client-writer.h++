// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-writer.h++
/// @brief Send log events to server over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "zmq-publisher.h++"
#include "zmq-protobuf-messagewriter.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    //==========================================================================
    // @class ClientWriter
    // @brief Send messages to Multilogger service over ZMQ

    class ClientWriter
        : public core::zmq::ProtoBufMessageWriter<cc::platform::multilogger::protobuf::Loggable>,
          public core::types::enable_create_shared_from_this<ClientWriter>
    {
        using This  = ClientWriter;
        using Super = core::zmq::ProtoBufMessageWriter<cc::platform::multilogger::protobuf::Loggable>;

    protected:
        ClientWriter(const std::shared_ptr<core::zmq::Publisher> &publisher);

    public:
        void write(const core::types::Loggable::ptr &loggable);

    private:
        std::shared_ptr<core::zmq::Publisher> publisher;
    };
}

