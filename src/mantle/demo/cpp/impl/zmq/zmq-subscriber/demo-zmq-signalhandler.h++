// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-signalhandler.h++"
#include "types/create-shared.h++"

#include "cc/protobuf/demo/demo.pb.h"

namespace demo::zmq
{
    class SignalHandler : public core::zmq::ProtoBufSignalHandler<cc::demo::Signal>,
                          public core::types::enable_create_shared<SignalHandler>
    {
        using This = SignalHandler;
        using Super = core::zmq::ProtoBufSignalHandler<cc::demo::Signal>;

    protected:
        void initialize() override;
        void handle_message(const cc::demo::Signal &message) override;
    };

}  // namespace demo::zmq
