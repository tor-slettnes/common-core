// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-writer.h++
/// @brief Send log events to server over ZMQ
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "zmq-publisher.h++"
#include "zmq-protobuf-messagewriter.h++"
#include "types/create-shared.h++"

namespace cc::platform::multilogger::zmq
{
    //==========================================================================
    // @class ClientWriter
    // @brief Send messages to Multilogger service over ZMQ

    class ClientWriter
        : public cc::zmq::ProtoBufMessageWriter<cc::platform::multilogger::protobuf::Loggable>,
          public core::types::enable_create_shared_from_this<ClientWriter>
    {
        using This = ClientWriter;
        using Super = cc::zmq::ProtoBufMessageWriter<cc::platform::multilogger::protobuf::Loggable>;

    protected:
        ClientWriter(const std::shared_ptr<cc::zmq::Publisher>& publisher);

    public:
        void write(const core::types::Loggable::ptr& loggable);

    private:
        std::shared_ptr<cc::zmq::Publisher> publisher;
    };
}  // namespace cc::platform::multilogger::zmq
