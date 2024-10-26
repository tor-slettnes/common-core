// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-requesthandler.c++
/// @brief Handle Logger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-requesthandler.h++"

namespace logger
{
    RequestHandler::RequestHandler(const std::shared_ptr<BaseLogger>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::log(
        ::grpc::ServerContext* context,
        const ::cc::logger::LogRecord* request,
        ::cc::logger::LogResponse* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader< ::cc::logger::LogRecord>* reader,
        ::cc::logger::LogResponse* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(),
                                 "writing to log");
        }
    }

    ::grpc::Status RequestHandler::listen(
        ::grpc::ServerContext* context,
        const ::cc::logger::LogFilter* request,
        ::grpc::ServerWriter< ::cc::logger::LogRecord>* writer)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::add_contract(
        ::grpc::ServerContext* context,
        const ::cc::logger::Contract* request,
        ::cc::logger::AddContractResponse* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::remove_contract(
        ::grpc::ServerContext* context,
        const ::cc::logger::ContractID* request,
        ::cc::logger::RemoveContractResponse* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_contract(
        ::grpc::ServerContext* context,
        const ::cc::logger::ContractID* request,
        ::cc::logger::Contract* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_static_fields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::logger::FieldNames* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_contracts(
        ::grpc::ServerContext* context,
        const ::cc::logger::ContractFilter* request,
        ::cc::logger::Contracts* response)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

}  // namespace logger
