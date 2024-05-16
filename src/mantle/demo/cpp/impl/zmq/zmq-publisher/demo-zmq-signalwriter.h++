// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalwriter.h++
/// @brief Forward local DEMO signals over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "zmq-protobuf-signalwriter.h++"

#include "types/create-shared.h++"

#include "demo.pb.h"

namespace demo::zmq
{
    //==========================================================================
    // @class SignalWriter
    // @brief Connect to local DEMO signals and write via ZMQ

    class SignalWriter : public cc::zmq::ProtoBufSignalWriter<cc::protobuf::demo::Signal>,
                         public cc::types::enable_create_shared<SignalWriter>
    {
        // Convencience alias
        using This = SignalWriter;
        using Super = cc::zmq::ProtoBufSignalWriter<cc::protobuf::demo::Signal>;

    protected:
        using Super::Super;

    public:
        void initialize() override;
        void deinitialize() override;
    };
}  // namespace demo::zmq
