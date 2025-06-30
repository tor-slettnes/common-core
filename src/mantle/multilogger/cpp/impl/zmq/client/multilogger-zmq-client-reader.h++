// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-reader.h++
/// @brief Read log events from server over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "zmq-protobuf-messagehandler.h++"
#include "zmq-subscriber.h++"
#include "thread/blockingqueue.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    //==========================================================================
    // @class ClientReader
    // @brief Handle log events received over ZMQ

    class ClientReader
        : public core::zmq::ProtoBufMessageHandler<cc::platform::multilogger::Loggable>,
          public core::types::BlockingQueue<core::types::Loggable::ptr>,
          public core::types::enable_create_shared<ClientReader>
    {
        using This = ClientReader;
        using HandlerBase = core::zmq::ProtoBufMessageHandler<cc::platform::multilogger::Loggable>;
        using QueueBase = core::types::BlockingQueue<core::types::Loggable::ptr>;

    protected:
        ClientReader(const std::weak_ptr<core::zmq::Subscriber> &subscriber);

    public:
        void deinitialize() override;

    public:
        void handle_message(const cc::platform::multilogger::Loggable &msg) override;
    };
}  // namespace multilogger::zmq
