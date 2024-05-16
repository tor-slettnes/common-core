// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-signalhandler.h++"
#include "types/create-shared.h++"

#include "demo.pb.h"

namespace demo::zmq
{
    class SignalHandler : public cc::zmq::ProtoBufSignalHandler<cc::protobuf::demo::Signal>,
                          public cc::types::enable_create_shared<SignalHandler>
    {
        using This = SignalHandler;
        using Super = cc::zmq::ProtoBufSignalHandler<cc::protobuf::demo::Signal>;

    protected:
        void initialize() override;
        void handle_message(const cc::protobuf::demo::Signal &message) override;
    };

}  // namespace demo::zmq
