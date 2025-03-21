// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-requesthandler.h++
/// @brief Handle MultiLogger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "grpc-requesthandler.h++"

#include "multilogger.grpc.pb.h"  // generated from `multilogger.proto`

#include "types/create-shared.h++"

namespace multilogger::grpc
{
    class RequestHandler
        : public core::grpc::RequestHandler<cc::multilogger::MultiLogger>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::multilogger::MultiLogger>;

    protected:
        RequestHandler(const std::shared_ptr<API>& provider);

    public:
        ::grpc::Status submit(
            ::grpc::ServerContext* context,
            const ::cc::multilogger::Loggable* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status writer(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<::cc::multilogger::Loggable>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status listen(
            ::grpc::ServerContext* context,
            const ::cc::multilogger::ListenerSpec* request,
            ::grpc::ServerWriter<::cc::multilogger::Loggable>* writer) override;

        ::grpc::Status add_sink(
            ::grpc::ServerContext* context,
            const ::cc::multilogger::SinkSpec* request,
            ::cc::multilogger::AddSinkResult* response) override;

        ::grpc::Status remove_sink(
            ::grpc::ServerContext* context,
            const ::cc::multilogger::SinkID* request,
            ::cc::multilogger::RemoveSinkResult* response) override;

        ::grpc::Status get_sink(
            ::grpc::ServerContext* context,
            const ::cc::multilogger::SinkID* request,
            ::cc::multilogger::SinkSpec* response) override;

        ::grpc::Status get_all_sinks(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::multilogger::SinkSpecs* response) override;

        ::grpc::Status list_sinks(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::multilogger::SinkNames* response) override;

        ::grpc::Status list_sink_types(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::multilogger::SinkTypes* response) override;

        ::grpc::Status list_message_fields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::multilogger::FieldNames* response) override;

        ::grpc::Status list_error_fields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::multilogger::FieldNames* response) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace multilogger::grpc
