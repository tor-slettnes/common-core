/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-signalhandler.h++
/// @brief Handle received messages with a ProtoBuf Signal payload
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-messagehandler.h++"
#include "protobuf-signalreceiver.h++"
#include "logging/logging.h++"

namespace cc::zmq
{
    template <class SignalT>
    class ProtoBufSignalHandler : public ProtoBufMessageHandler<SignalT>,
                                  public cc::io::proto::SignalReceiver<SignalT>
    {
        using This = ProtoBufSignalHandler<SignalT>;
        using Super = ProtoBufMessageHandler<SignalT>;
        using SignalStore = cc::io::proto::SignalReceiver<SignalT>;

    protected:
        using Super::Super;

        void initialize() override
        {
            Super::initialize();
            SignalStore::initialize();
        }

        void deinitialize() override
        {
            SignalStore::deinitialize();
            Super::deinitialize();
        }

        void handle_message(const SignalT &message) override
        {
            SignalStore::process_signal(message);
        }
    };

}  // namespace cc::zmq
