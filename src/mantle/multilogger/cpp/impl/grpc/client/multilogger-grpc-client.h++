// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-client.h++
/// @brief Log gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger.grpc.pb.h"  // Generated from `multilogger.proto`
#include "multilogger-api.h++"
#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"
#include "logging/sinks/sink.h++"

namespace multilogger::grpc
{
    using LogClientBase = core::grpc::ClientWrapper<cc::platform::multilogger::MultiLogger>;

    class LogClient
        : public API,
          public LogClientBase,
          public core::logging::Sink,
          public core::types::enable_create_shared_from_this<LogClient>
    {
        using This = LogClient;

    protected:
        template <class... Args>
        LogClient(const std::string &identity,
                  const std::string &host = "",
                  bool add_local_sink = true,
                  Args &&...args)
            : API(identity),
              LogClientBase(host, std::forward<Args>(args)...),
              Sink(this->host() + ":" + identity, true),
              add_local_sink(add_local_sink)
        {
        }

    public:
        void initialize() override;
        void deinitialize() override;

        void submit(const core::types::Loggable::ptr &item) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs get_all_sink_specs() const override;
        SinkIDs list_sinks() const override;
        SinkTypes list_sink_types() const override;
        FieldNames list_message_fields() const override;
        FieldNames list_error_fields() const override;

        std::shared_ptr<LogSource> listen(
            const ListenerSpec &spec) override;

    protected:
        void open() override;
        void close() override;
        bool handle_item(const core::types::Loggable::ptr &item) override;

    private:
        bool add_local_sink;
        std::unique_ptr<::grpc::ClientWriter<::cc::platform::multilogger::Loggable>> writer;
        std::unique_ptr<::grpc::ClientContext> writer_context;
        std::unique_ptr<::google::protobuf::Empty> writer_response;
        core::grpc::Status writer_status;
    };
}  // namespace multilogger::grpc
