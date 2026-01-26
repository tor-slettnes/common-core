// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes
//==============================================================================

#include "vfs-grpc-signalqueue.h++"
#include "vfs-signals.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-inline.h++"

namespace vfs::grpc
{
    void SignalQueue::initialize()
    {
        this->connect<Context::ptr>(
            vfs::protobuf::Signal::kContext,
            vfs::signal_context,
            [=](Context::ptr ref, vfs::protobuf::Signal *msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_context());
            });

        this->connect<Context::ptr>(
            vfs::protobuf::Signal::kContextInUse,
            vfs::signal_context_in_use,
            [=](Context::ptr ref, vfs::protobuf::Signal *msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_context_in_use());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(vfs::signal_context_in_use);
        this->disconnect(vfs::signal_context);
        Super::deinitialize();
    }
}  // namespace vfs::grpc
