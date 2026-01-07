// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-client.h++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/relay/protobuf/relay_types.pb.h"
#include "cc/platform/relay/grpc/relay_service.grpc.pb.h"

#include "relay-grpc-reader.h++"
#include "relay-publisher.h++"
#include "relay-subscriber.h++"

#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"

namespace relay::grpc
{
    class Client
        : public relay::Publisher,
          public relay::Subscriber,
          public core::grpc::ClientWrapper<cc::platform::relay::grpc::Relay>,
          public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using ClientBase = core::grpc::ClientWrapper<cc::platform::relay::grpc::Relay>;
        using Message = cc::platform::relay::protobuf::Message;
        using MessageReceiver = std::function<void(std::string, core::types::Value)>;

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

        std::unique_ptr<::grpc::ClientWriter<Message>> writer_;
        std::unique_ptr<::grpc::ClientContext> writer_context_;
        std::unique_ptr<::google::protobuf::Empty> writer_response_;
        core::grpc::Status writer_status_;
    };
}  // namespace relay::grpc
