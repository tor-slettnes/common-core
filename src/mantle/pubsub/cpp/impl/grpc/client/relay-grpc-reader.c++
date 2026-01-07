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

namespace relay::grpc
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

    std::optional<MessageData> Reader::get()
    {
        cc::platform::pubsub::protobuf::Message msg;
        if (this->reader->Read(&msg))
        {
            return MessageData(
                msg.topic(),
                protobuf::decoded<core::types::Value>(msg.payload()));
        }
        else
        {
            this->reader->Finish();
            return {};
        }
    }

}  // namespace relay::grpc
