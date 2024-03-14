// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-signalhandler.h++"
#include "types/create-shared.h++"

#include "demo_types.pb.h"

namespace demo::zmq
{
    class SignalHandler : public core::zmq::ProtoBufSignalHandler<CC::Demo::Signal>,
                          public core::types::enable_create_shared<SignalHandler>
    {
        using This = SignalHandler;
        using Super = core::zmq::ProtoBufSignalHandler<CC::Demo::Signal>;

    protected:
        void initialize() override;
        void handle_message(const CC::Demo::Signal &message) override;
    };

}  // namespace demo::zmq
