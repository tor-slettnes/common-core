// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-signalqueue.h++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/sysconfig/protobuf/sysconfig_types.pb.h"  // generated from `sysconfig.proto`

#include "grpc-signalqueue.h++"

namespace sysconfig::grpc
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

    class SignalQueue : public core::grpc::SignalQueue<::cc::platform::sysconfig::protobuf::Signal>
    {
        using Super = core::grpc::SignalQueue<::cc::platform::sysconfig::protobuf::Signal>;

    public:
        using Super::Super;
        void initialize() override;
        void deinitialize() override;
    };
}  // namespace sysconfig::grpc
