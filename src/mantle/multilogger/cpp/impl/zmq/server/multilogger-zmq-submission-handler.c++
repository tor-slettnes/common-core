// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-submission-handler.c+
/// @brief Handle log events received from clients over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-submission-handler.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    SubmissionHandler::SubmissionHandler(
        const std::shared_ptr<API> &provider,
        const std::shared_ptr<core::zmq::Subscriber> &subscriber)
        : Super(),
          provider(provider),
          subscriber(subscriber)
    {
    }

    SubmissionHandler::~SubmissionHandler()
    {
        this->deinitialize();
    }

    void SubmissionHandler::initialize()
    {
        Super::initialize();
        this->subscriber->register_handler(this->weak_from_this());
    }

    void SubmissionHandler::deinitialize()
    {
        this->subscriber->unregister_handler(this->weak_from_this());
        Super::deinitialize();
    }

    void SubmissionHandler::handle_message(const cc::platform::multilogger::Loggable &msg)
    {
        this->provider->submit(protobuf::decode_loggable(msg));
    }
}  // namespace multilogger::zmq
