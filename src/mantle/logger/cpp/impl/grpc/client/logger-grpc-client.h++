// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-client.h++
/// @brief Logger gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger.grpc.pb.h"  // Generated from `logger.proto`
#include "logger-api.h++"
#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"
#include "logging/sinks/asynclogsink.h++"

namespace logger::grpc
{
    using LoggerClientBase = core::grpc::ClientWrapper<cc::logger::Logger>;

    class LoggerClient
        : public API,
          public LoggerClientBase,
          public core::logging::AsyncLogSink,
          public core::types::enable_create_shared_from_this<LoggerClient>
    {
        using This = LoggerClient;

    protected:
        template <class... Args>
        LoggerClient(const std::string &identity,
                     const std::string &host = "",
                     bool add_local_sink = true,
                     Args &&...args)
            : API(identity),
              LoggerClientBase(host, std::forward<Args>(args)...),
              AsyncLogSink(this->host()),
              add_local_sink(add_local_sink)
        {
        }

    public:
        void initialize() override;
        void deinitialize() override;

        void submit(const core::status::Event::ptr &event) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs get_all_sink_specs() const override;
        SinkIDs list_sinks() const override;
        SinkTypes list_sink_types() const override;
        FieldNames list_static_fields() const override;

        std::shared_ptr<EventSource> listen(
            const ListenerSpec &spec) override;

    protected:
        void open() override;
        void close() override;
        void capture_event(const core::status::Event::ptr &event) override;

    private:
        bool add_local_sink;
        std::unique_ptr<::grpc::ClientWriter<::cc::status::Event>> writer;
        std::unique_ptr<::grpc::ClientContext> writer_context;
        std::unique_ptr<::google::protobuf::Empty> writer_response;
        core::grpc::Status writer_status;

    };
}  // namespace logger::grpc
