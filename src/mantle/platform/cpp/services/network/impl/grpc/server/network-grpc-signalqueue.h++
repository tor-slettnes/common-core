// -*- c++ -*-
//==============================================================================
/// @file nework-grpc-signalqueue.h++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#pragma once
#include "network.pb.h"  // generated from `network.proto`
#include "grpc-signalqueue.h++"

namespace platform::network::grpc
{
    //==========================================================================
    /// @class SignalQueue
    /// @brief Connect local signals to a single client over gRPC
    ///
    /// Each SignalQueue instance is associated with a specific invocation of
    /// the server's `watch()` method, and captures signals on the server side
    /// to be streamed back to the client. We connect the desired signals to
    /// corresponding handler methods, which in turn encode the payload to our
    /// own protobuf Signal() message and then appends the result to this queue,
    /// from which they are then streamed back to the client.
    ///
    /// See `core::grpc::SignalQueue<T>` for additional info.

    class SignalQueue : public core::grpc::SignalQueue<cc::platform::network::Signal>
    {
        using Super = core::grpc::SignalQueue<cc::platform::network::Signal>;

    public:
        using Super::Super;
        void initialize() override;
        void deinitialize() override;
    };
}
