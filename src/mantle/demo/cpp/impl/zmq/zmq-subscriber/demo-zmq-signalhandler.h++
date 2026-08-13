// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-protobuf-signalhandler.h++"
#include "types/create-shared.h++"

#include "cc/demo/protobuf/demo_types.pb.h"

namespace cc::demo::zmq
{
    class SignalHandler : public cc::zmq::ProtoBufSignalHandler<cc::demo::protobuf::Signal>,
                          public core::types::enable_create_shared<SignalHandler>
    {
        using This = SignalHandler;
        using Super = cc::zmq::ProtoBufSignalHandler<cc::demo::protobuf::Signal>;

    protected:
        void initialize() override;
        void handle_message(const cc::demo::protobuf::Signal &message) override;
    };

}  // namespace cc::demo::zmq
