// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-requesthandler.h++
/// @brief Handle MultiLogger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "grpc-requesthandler.h++"

#include "cc/platform/multilogger/grpc/multilogger_service.grpc.pb.h"

#include "types/create-shared.h++"

namespace multilogger::grpc
{
    class RequestHandler
        : public core::grpc::RequestHandler<cc::platform::multilogger::grpc::MultiLogger>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::platform::multilogger::grpc::MultiLogger>;

    protected:
        RequestHandler(const std::shared_ptr<API>& provider);

    public:
        ::grpc::Status Submit(
            ::grpc::ServerContext* context,
            const ::cc::platform::multilogger::protobuf::Loggable* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Writer(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<::cc::platform::multilogger::protobuf::Loggable>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Listen(
            ::grpc::ServerContext* context,
            const ::cc::platform::multilogger::protobuf::ListenerSpec* request,
            ::grpc::ServerWriter<::cc::platform::multilogger::protobuf::Loggable>* writer) override;

        ::grpc::Status AddSink(
            ::grpc::ServerContext* context,
            const ::cc::platform::multilogger::protobuf::SinkSpec* request,
            ::cc::platform::multilogger::protobuf::AddSinkResult* response) override;

        ::grpc::Status RemoveSink(
            ::grpc::ServerContext* context,
            const ::cc::platform::multilogger::protobuf::SinkID* request,
            ::cc::platform::multilogger::protobuf::RemoveSinkResult* response) override;

        ::grpc::Status GetSink(
            ::grpc::ServerContext* context,
            const ::cc::platform::multilogger::protobuf::SinkID* request,
            ::cc::platform::multilogger::protobuf::SinkSpec* response) override;

        ::grpc::Status GetAllSinks(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::multilogger::protobuf::SinkSpecs* response) override;

        ::grpc::Status ListSinks(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::multilogger::protobuf::SinkNames* response) override;

        ::grpc::Status ListSinkTypes(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::multilogger::protobuf::SinkTypes* response) override;

        ::grpc::Status ListMessageFields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::multilogger::protobuf::FieldNames* response) override;

        ::grpc::Status ListErrorFields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::multilogger::protobuf::FieldNames* response) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace multilogger::grpc
