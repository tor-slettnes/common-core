// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-writer.c++
/// @brief Send log events to server over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-client-writer.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{

    ClientWriter::ClientWriter(const std::shared_ptr<core::zmq::Publisher> &publisher)
        : Super(publisher)
    {
    }

    void ClientWriter::write(const core::types::Loggable::ptr &loggable)
    {
        Super::write(
            protobuf::encoded_shared<cc::platform::multilogger::Loggable>(
                loggable));
    }

}  // namespace multilogger::zmq
