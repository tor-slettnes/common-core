// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-reader.c++
/// @brief Read log events from server over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-client-reader.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    ClientReader::ClientReader(const std::weak_ptr<core::zmq::Subscriber> &subscriber)
        : HandlerBase({}, subscriber),
          QueueBase()
    {
    }

    void ClientReader::deinitialize()
    {
        HandlerBase::deinitialize();
        QueueBase::close();
    }

    void ClientReader::handle_message(const cc::platform::multilogger::protobuf::Loggable &msg)
    {
        this->put(protobuf::decode_loggable(msg));
    }
}  // namespace multilogger::zmq
