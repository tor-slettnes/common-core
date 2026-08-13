// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-requesthandler.h++
/// @brief Handle MultiLogger RPC requests
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "grpc-requesthandler.h++"

#include "cc/platform/multilogger/grpc/multilogger_service.grpc.pb.h"

#include "types/create-shared.h++"

namespace cc::platform::multilogger::grpc
{
    class RequestHandler
        : public cc::grpc::RequestHandler<cc::platform::multilogger::grpc::MultiLogger>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = cc::grpc::RequestHandler<cc::platform::multilogger::grpc::MultiLogger>;

    protected:
        RequestHandler(const std::shared_ptr<API> &provider);

    public:
        ::grpc::Status ServiceCheck(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            ServiceCheckResponse *response) override;

        ::grpc::Status Submit(
            ::grpc::ServerContext *context,
            const protobuf::Loggable *request,
            ::google::protobuf::Empty *response) override;

        ::grpc::Status Writer(
            ::grpc::ServerContext *context,
            ::grpc::ServerReader<protobuf::Loggable> *reader,
            ::google::protobuf::Empty *response) override;

        ::grpc::Status Listen(
            ::grpc::ServerContext *context,
            const protobuf::ListenerSpec *request,
            ::grpc::ServerWriter<protobuf::Loggable> *writer) override;

        ::grpc::Status AddSink(
            ::grpc::ServerContext *context,
            const protobuf::SinkSpec *request,
            protobuf::AddSinkResult *response) override;

        ::grpc::Status RemoveSink(
            ::grpc::ServerContext *context,
            const protobuf::SinkID *request,
            protobuf::RemoveSinkResult *response) override;

        ::grpc::Status GetSink(
            ::grpc::ServerContext *context,
            const protobuf::SinkID *request,
            protobuf::SinkSpec *response) override;

        ::grpc::Status GetAllSinks(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            protobuf::SinkSpecs *response) override;

        ::grpc::Status ListSinks(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            protobuf::SinkNames *response) override;

        ::grpc::Status ListSinkTypes(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            protobuf::SinkTypes *response) override;

        ::grpc::Status ListMessageFields(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            protobuf::FieldNames *response) override;

        ::grpc::Status ListErrorFields(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            protobuf::FieldNames *response) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace cc::platform::multilogger::grpc
