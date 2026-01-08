// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-reader.c++
/// @brief Stream publications from broker
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-reader.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

#include "cc/platform/pubsub/grpc/relay_service.grpc.pb.h"

namespace pubsub::grpc
{
    //--------------------------------------------------------------------------
    // Reader

    Reader::Reader(
        const std::unique_ptr<::cc::platform::pubsub::grpc::Relay::Stub> &stub,
        const ::cc::platform::pubsub::protobuf::Filters &filters)
        : reader(stub->Subscriber(&this->context, filters))
    {
    }

    Reader::~Reader()
    {
        this->close();
    }

    void Reader::close()
    {
        this->context.TryCancel();
    }

    std::optional<pubsub::MessageItem> Reader::get()
    {
        cc::platform::pubsub::protobuf::Publication msg;

        if (this->reader->Read(&msg))
        {
            return pubsub::MessageItem(
                msg.topic(),
                protobuf::decoded<core::types::Value>(msg.value()));
        }
        else
        {
            this->reader->Finish();
            return {};
        }
    }

}  // namespace pubsub::grpc
