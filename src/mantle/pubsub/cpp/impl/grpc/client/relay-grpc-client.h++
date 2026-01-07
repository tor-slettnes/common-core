// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-client.h++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/pubsub/grpc/relay_service.grpc.pb.h"

#include "relay-grpc-reader.h++"
#include "relay-publisher.h++"
#include "relay-subscriber.h++"

#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"

namespace pubsub::grpc
{
    class Client
        : public pubsub::Publisher,
          public pubsub::Subscriber,
          public core::grpc::ClientWrapper<cc::platform::pubsub::grpc::Relay>,
          public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using ClientBase = core::grpc::ClientWrapper<cc::platform::pubsub::grpc::Relay>;
        using ClientWriter = ::grpc::ClientWriter<cc::platform::pubsub::protobuf::Publication>;

    protected:
        template <class... Args>
        Client(const std::string &host = "", Args &&...args)
            : ClientBase(host, std::forward<Args>(args)...)
        {
        }

    public:
        void initialize() override;
        void deinitialize() override;
        bool write(const std::string &topic,
                   const core::types::Value &payload) override;

    private:
        void start_writer() override;
        void stop_writer() override;

        void start_reader() override;
        void stop_reader() override;

        Reader::ptr create_reader(
            const std::vector<Topic> &topics = {});

        void read_worker();

    private:
        std::thread reader_thread_;
        std::shared_ptr<Reader> reader_;

        std::unique_ptr<ClientWriter> writer_;
        std::unique_ptr<::grpc::ClientContext> writer_context_;
        std::unique_ptr<::google::protobuf::Empty> writer_response_;
        core::grpc::Status writer_status_;
    };
}  // namespace pubsub::grpc
