// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-requesthandler.h++
/// @brief Handle Logger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-base.h++"
#include "grpc-requesthandler.h++"

#include "logger.grpc.pb.h"  // generated from `logger.proto`

#include "types/create-shared.h++"

namespace logger
{
    class RequestHandler : public core::grpc::RequestHandler<cc::logger::Logger>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::logger::Logger>;

    protected:
        RequestHandler(const std::shared_ptr<BaseLogger>& provider);

    public:
        ::grpc::Status log(
            ::grpc::ServerContext* context,
            const ::cc::logger::LogRecord* request,
            ::cc::logger::LogResponse* response) override;

        ::grpc::Status writer(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader< ::cc::logger::LogRecord>* reader,
            ::cc::logger::LogResponse* response) override;

        ::grpc::Status listen(
            ::grpc::ServerContext* context,
            const ::cc::logger::LogFilter* request,
            ::grpc::ServerWriter< ::cc::logger::LogRecord>* writer) override;

        ::grpc::Status add_contract(
            ::grpc::ServerContext* context,
            const ::cc::logger::Contract* request,
            ::cc::logger::AddContractResponse* response) override;

        ::grpc::Status remove_contract(
            ::grpc::ServerContext* context,
            const ::cc::logger::ContractID* request,
            ::cc::logger::RemoveContractResponse* response) override;

        ::grpc::Status get_contract(
            ::grpc::ServerContext* context,
            const ::cc::logger::ContractID* request,
            ::cc::logger::Contract* response) override;

        ::grpc::Status get_static_fields(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::logger::FieldNames* response) override;

        ::grpc::Status list_contracts(
            ::grpc::ServerContext* context,
            const ::cc::logger::ContractFilter* request,
            ::cc::logger::Contracts* response) override;

    private:
        std::shared_ptr<BaseLogger> provider;
    };
}  // namespace logger
