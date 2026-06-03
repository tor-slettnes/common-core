// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-reader.h++
/// @brief Stream publications from broker
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "types/create-shared.h++"

#include "cc/platform/pubsub/grpc/relay_service.grpc.pb.h"
#include "cc/protobuf/status/status.pb.h"

namespace cc::platform::pubsub::grpc
{
    //--------------------------------------------------------------------------
    // Reader

    class Reader : public MessageSource,
                   public core::types::enable_create_shared<Reader>
    {
    protected:
        Reader(const std::unique_ptr<platform::pubsub::grpc::Relay::Stub>& stub,
               const platform::pubsub::protobuf::Filters &filters);

    public:
        ~Reader();

    public:
        void close() override;
        std::optional<MessageItem> get() override;

    private:
        ::grpc::ClientContext context;
        std::unique_ptr<::grpc::ClientReader<platform::pubsub::protobuf::Publication>> reader;
    };
}  // namespace cc::platform::pubsub::grpc
