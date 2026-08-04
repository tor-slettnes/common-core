// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-base-client.h++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/pubsub/grpc/relay_service.grpc.pb.h"

#include "relay-grpc-reader.h++"
#include "relay-publisher.h++"
#include "relay-subscriber.h++"
#include "relay-control.h++"

#include "grpc-clientwrapper.h++"

namespace cc::platform::pubsub::grpc
{
    //--------------------------------------------------------------------------
    // ClientImpl -- Inheritable base

    class ClientImpl
        : public pubsub::Publisher,
          public pubsub::Subscriber,
          public cc::grpc::ClientWrapper<cc::platform::pubsub::grpc::Relay>
    {
        using This = ClientImpl;
        using ClientBase = cc::grpc::ClientWrapper<cc::platform::pubsub::grpc::Relay>;
        using ClientWriter = ::grpc::ClientWriter<cc::platform::pubsub::protobuf::Publication>;

    protected:
        template <class... Args>
        ClientImpl(const std::string &host = "", Args &&...args)
            : ClientBase(host, std::forward<Args>(args)...)
        {
        }

    public:
        void initialize() override;
        void deinitialize() override;
        bool write(const std::string &topic,
                   const core::types::Value &value) override;

        void start_writer() override;
        void stop_writer() override;

        void start_reader() override;
        void stop_reader() override;

    private:
        Reader::ptr create_reader(
            const std::vector<Topic> &topics = {});

        void read_worker();

    private:
        std::thread reader_thread_;
        std::shared_ptr<Reader> reader_;

        std::unique_ptr<ClientWriter> writer_;
        std::unique_ptr<::grpc::ClientContext> writer_context_;
        std::unique_ptr<::google::protobuf::Empty> writer_response_;
        cc::grpc::Status writer_status_;
    };

}  // namespace cc::platform::pubsub::grpc
