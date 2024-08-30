// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "vfs-grpc-signalqueue.h++"
#include "vfs-signals.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-inline.h++"

namespace platform::vfs::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::vfs::Signal;

        this->connect<ContextRef>(
            Signal::kContext,
            platform::vfs::signal_context,
            [=](ContextRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_context());
            });

        this->connect<ContextRef>(
            Signal::kContextInUse,
            platform::vfs::signal_context_in_use,
            [=](ContextRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_context_in_use());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(platform::vfs::signal_context_in_use);
        this->disconnect(platform::vfs::signal_context);
        Super::deinitialize();
    }
}  // namespace platform::vfs::grpc
