// -*- c++ -*-
//==============================================================================
/// @file system-grpc-signalqueue.h++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#pragma once
#include "system.pb.h"  // generated from `foup.proto`

#include "grpc-signalqueue.h++"

namespace platform::system::grpc
{
    class SignalQueue : public core::grpc::SignalQueue<cc::platform::system::Signal>
    {
        using Super = core::grpc::SignalQueue<cc::platform::system::Signal>;

    public:
        using Super::Super;
        void initialize() override;
        void deinitialize() override;
    };
}  // namespace sam::foup::grpc
