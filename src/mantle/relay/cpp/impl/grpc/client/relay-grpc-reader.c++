// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-reader.c++
/// @brief Stream publications from broker
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-reader.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

#include "cc/platform/relay/grpc/relay_service.grpc.pb.h"

namespace relay::grpc
{
    //--------------------------------------------------------------------------
    // Reader

    Reader::Reader(
        const std::unique_ptr<::cc::platform::relay::grpc::Relay::Stub> &stub,
        const ::cc::platform::relay::protobuf::Filters &filters)
        : reader(stub->Reader(&this->context, filters))
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
        cc::platform::relay::protobuf::Message msg;
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
