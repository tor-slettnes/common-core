// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-submission-handler.h++
/// @brief Handle log events received from clients over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "multilogger-api.h++"
#include "zmq-protobuf-messagehandler.h++"
#include "zmq-subscriber.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    class SubmissionHandler
        : public core::zmq::ProtoBufMessageHandler<cc::platform::multilogger::protobuf::Loggable>,
          public core::types::enable_create_shared<SubmissionHandler>
    {
        using This = SubmissionHandler;
        using Super = core::zmq::ProtoBufMessageHandler<cc::platform::multilogger::protobuf::Loggable>;

    protected:
        SubmissionHandler(const std::shared_ptr<API> &provider,
                          const std::weak_ptr<core::zmq::Subscriber> &subscriber);

    protected:
        void handle_message(const cc::platform::multilogger::protobuf::Loggable &msg) override;

    private:
        std::shared_ptr<API> provider;
    };
}

