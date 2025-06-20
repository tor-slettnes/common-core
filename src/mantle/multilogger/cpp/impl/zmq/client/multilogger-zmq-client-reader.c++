// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-reader.c++
/// @brief Read log events from server over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-client-reader.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    ClientReader::ClientReader(const std::shared_ptr<core::zmq::Subscriber> &subscriber)
        : HandlerBase(),
          QueueBase(),
          subscriber(subscriber)
    {
    }

    ClientReader::~ClientReader()
    {
        this->deinitialize();
    }

    void ClientReader::initialize()
    {
        HandlerBase::initialize();
        this->subscriber->register_handler(this->weak_from_this());
    }

    void ClientReader::deinitialize()
    {
        if (!this->closed())
        {
            this->subscriber->unregister_handler(this->weak_from_this());
            HandlerBase::deinitialize();
            QueueBase::close();
        }
    }

    void ClientReader::handle_message(const cc::platform::multilogger::Loggable &msg)
    {
        this->put(protobuf::decode_loggable(msg));
    }
}  // namespace multilogger::zmq
