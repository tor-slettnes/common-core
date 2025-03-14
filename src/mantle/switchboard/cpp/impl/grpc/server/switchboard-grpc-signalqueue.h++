// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-signalqueue.h++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-signalqueue.h++"
#include "switchboard.grpc.pb.h"

namespace switchboard::grpc
{
    //==========================================================================
    /// @class SignalQueue
    /// @brief Connect switchboard signals to a single client over gRPC
    ///
    /// Each SignalQueue instance is associated with a specific invocation of
    /// the server's `watch()` method, and captures signals on the server side
    /// to be streamed back to the client. We connect the desired signals to
    /// corresponding handler methods, which in turn encode the emitted payload
    /// to our own protobuf Signal() message and then appends the result to this
    /// queue, from which they are then streamed back to the client.
    ///
    /// See `core::grpc::SignalQueue<T>` for additional info.

    class SignalQueue : public core::grpc::SignalQueue<cc::switchboard::Signal>
    {
        using This = SignalQueue;
        using Super = core::grpc::SignalQueue<cc::switchboard::Signal>;

    public:
        using Super::Super;
        void initialize() override;
        void deinitialize() override;
    };
}  // namespace switchboard::grpc
