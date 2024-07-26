/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-signalwriter.h++
/// @brief Connect to local signals and forward as publications over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-messagewriter.h++"
#include "zmq-publisher.h++"
#include "protobuf-signalforwarder.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace core::zmq
{
    template <class ProtoT>
    class ProtoBufSignalWriter : public protobuf::SignalForwarder<ProtoT>,
                                 public ProtoBufMessageWriter<ProtoT>
    {
        using This = ProtoBufSignalWriter<ProtoT>;
        using Super = ProtoBufMessageWriter<ProtoT>;

    protected:
        using Super::Super;

        void initialize() override
        {
            protobuf::SignalForwarder<ProtoT>::initialize();
            ProtoBufMessageWriter<ProtoT>::initialize();
        }

        void deinitialize() override
        {
            ProtoBufMessageWriter<ProtoT>::deinitialize();
            protobuf::SignalForwarder<ProtoT>::deinitialize();
        }

        void forward(ProtoT &&message) override
        {
            this->write(message);
        }
    };

}  // namespace core::zmq
