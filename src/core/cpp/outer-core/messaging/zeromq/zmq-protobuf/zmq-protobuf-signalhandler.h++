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

namespace core::zmq
{
    template <class SignalT>
    class ProtoBufSignalHandler : public ProtoBufMessageHandler<SignalT>,
                                  public protobuf::SignalReceiver<SignalT>
    {
        using This = ProtoBufSignalHandler<SignalT>;
        using Super = ProtoBufMessageHandler<SignalT>;
        using SignalStore = protobuf::SignalReceiver<SignalT>;

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

}  // namespace core::zmq
