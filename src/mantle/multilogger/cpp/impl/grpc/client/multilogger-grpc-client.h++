// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-client.h++
/// @brief Log gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/multilogger/grpc/multilogger.grpc.pb.h"  // Generated from `multilogger.proto`
#include "multilogger-api.h++"
#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"
#include "logging/sinks/sink.h++"

namespace multilogger::grpc
{
    class ClientImpl
        : public API,
          public core::grpc::ClientWrapper<cc::platform::multilogger::grpc::MultiLogger>,
          public core::types::enable_create_shared_from_this<ClientImpl>
    {
        using This = ClientImpl;
        using Super = core::grpc::ClientWrapper<cc::platform::multilogger::grpc::MultiLogger>;

    protected:
        template <class... Args>
        ClientImpl(const std::string &host = "",
                   Args &&...args)
            : API(),
              Super(host, std::forward<Args>(args)...)
        {
        }

    public:
        void submit(const core::types::Loggable::ptr &item) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs get_all_sink_specs() const override;
        SinkIDs list_sinks() const override;
        SinkTypes list_sink_types() const override;
        FieldNames list_message_fields() const override;
        FieldNames list_error_fields() const override;

        // Create a listener to read loggable items from server.
        std::shared_ptr<LogSource> listen(
            const ListenerSpec &spec) override;

        // Stream loggable items to server
        bool is_writer_open() const;
        void open_writer();
        void close_writer();
        bool write_item(const core::types::Loggable::ptr &item);

    private:
        bool add_local_sink;
        std::unique_ptr<::grpc::ClientWriter<::cc::platform::multilogger::protobuf::Loggable>> writer;
        std::unique_ptr<::grpc::ClientContext> writer_context;
        std::unique_ptr<::google::protobuf::Empty> writer_response;
        core::grpc::Status writer_status;
    };
}  // namespace multilogger::grpc
