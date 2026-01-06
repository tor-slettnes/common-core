// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-reader.h++
/// @brief Stream publications from broker
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "types/create-shared.h++"

#include "cc/platform/relay/grpc/relay_service.grpc.pb.h"
#include "cc/platform/relay/protobuf/relay_types.pb.h"
#include "cc/protobuf/status/status.pb.h"

namespace relay::grpc
{
    //--------------------------------------------------------------------------
    // Reader

    class Reader : public MessageSource,
                   public core::types::enable_create_shared<Reader>
    {
    protected:
        Reader(const std::unique_ptr<::cc::platform::relay::grpc::Relay::Stub>& stub,
               const ::cc::platform::relay::protobuf::Filters &filters);

    public:
        ~Reader();

    public:
        void close() override;
        std::optional<MessageData> get() override;

    private:
        ::grpc::ClientContext context;
        std::unique_ptr<::grpc::ClientReader<::cc::platform::relay::protobuf::Message>> reader;
    };
}  // namespace relay::grpc
