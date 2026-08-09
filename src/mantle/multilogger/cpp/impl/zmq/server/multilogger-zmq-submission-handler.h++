// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-submission-handler.h++
/// @brief Handle log events received from clients over ZMQ
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "multilogger-api.h++"
#include "zmq-protobuf-messagehandler.h++"
#include "zmq-subscriber.h++"
#include "types/create-shared.h++"

namespace cc::platform::multilogger::zmq
{
    class SubmissionHandler
        : public cc::zmq::ProtoBufMessageHandler<cc::platform::multilogger::protobuf::Loggable>,
          public core::types::enable_create_shared<SubmissionHandler>
    {
        using This = SubmissionHandler;
        using Super = cc::zmq::ProtoBufMessageHandler<cc::platform::multilogger::protobuf::Loggable>;

    protected:
        SubmissionHandler(const std::shared_ptr<API>& provider,
                          const std::weak_ptr<cc::zmq::Subscriber>& subscriber);

    protected:
        void handle_message(const cc::platform::multilogger::protobuf::Loggable& msg) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace cc::platform::multilogger::zmq
