// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-requesthandler.h++
/// @brief Handle Logger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-api.h++"
#include "grpc-requesthandler.h++"

#include "logger.grpc.pb.h"  // generated from `logger.proto`

#include "types/create-shared.h++"

namespace logger::grpc
{
    class RequestHandler : public core::grpc::RequestHandler<cc::logger::Logger>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::logger::Logger>;

    protected:
        RequestHandler(const std::shared_ptr<API>& provider);

    public:
        ::grpc::Status log(
            ::grpc::ServerContext* context,
            const ::cc::status::Event* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status writer(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<::cc::status::Event>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status listen(
            ::grpc::ServerContext* context,
            const ::cc::logger::ListenerSpec* request,
            ::grpc::ServerWriter<::cc::status::Event>* writer) override;

        ::grpc::Status add_sink(
            ::grpc::ServerContext* context,
            const ::cc::logger::SinkSpec* request,
            ::cc::logger::AddSinkResult* response) override;

        ::grpc::Status remove_sink(
            ::grpc::ServerContext* context,
            const ::cc::logger::SinkID* request,
            ::cc::logger::RemoveSinkResult* response) override;

        ::grpc::Status get_sink(
            ::grpc::ServerContext* context,
            const ::cc::logger::SinkID* request,
            ::cc::logger::SinkSpec* response) override;

        ::grpc::Status list_sinks(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::logger::SinkSpecs* response) override;

        ::grpc::Status list_static_fields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::logger::FieldNames* response) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace logger::grpc
