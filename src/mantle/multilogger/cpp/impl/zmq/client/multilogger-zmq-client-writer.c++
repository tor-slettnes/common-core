// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-writer.c++
/// @brief Send log events to server over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-client-writer.h++"
#include "protobuf-inline.h++"

namespace cc::platform::multilogger::zmq
{

    ClientWriter::ClientWriter(const std::shared_ptr<cc::zmq::Publisher>& publisher)
        : Super(publisher)
    {
    }

    void ClientWriter::write(const core::types::Loggable::ptr& loggable)
    {
        Super::write(
            cc::protobuf::encoded_shared<cc::platform::multilogger::protobuf::Loggable>(
                loggable));
    }

}  // namespace cc::platform::multilogger::zmq
