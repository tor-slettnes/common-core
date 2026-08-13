// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-submission-handler.c++
/// @brief Handle log events received from clients over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-submission-handler.h++"
#include "protobuf-inline.h++"

namespace cc::platform::multilogger::zmq
{
    SubmissionHandler::SubmissionHandler(
        const std::shared_ptr<API> &provider,
        const std::weak_ptr<cc::zmq::Subscriber> &subscriber)
        : Super({}, subscriber),
          provider(provider)
    {
    }

    void SubmissionHandler::handle_message(
        const cc::platform::multilogger::protobuf::Loggable &msg)
    {
        this->provider->submit(cc::protobuf::decode_loggable(msg));
    }
}  // namespace cc::platform::multilogger::zmq
