// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-requesthandler.h++
/// @brief Handle Upgrade gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-requesthandler.h++"
#include "upgrade-grpc-signalqueue.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace platform::upgrade::grpc
{
    RequestHandler::RequestHandler()
        : provider(platform::upgrade::upgrade.get_shared())
    {
    }

    RequestHandler::RequestHandler(
        const std::shared_ptr<platform::upgrade::ProviderInterface>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::scan(
        ::grpc::ServerContext* context,
        const ::cc::platform::upgrade::PackageSource* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->scan(protobuf::decoded<PackageSource>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_available(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::grpc::ServerWriter<::cc::platform::upgrade::PackageInfo> *writer)
    {
        try
        {
            for (const PackageInfo::Ref& info : this->provider->list_available())
            {
                if (context->IsCancelled())
                {
                    break;
                }

                writer->Write(
                    protobuf::encode_shared<::cc::platform::upgrade::PackageInfo>(info));
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::best_available(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::upgrade::PackageInfo* response)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->best_available(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::install(
        ::grpc::ServerContext* context,
        const ::cc::platform::upgrade::PackageSource* request,
        ::cc::platform::upgrade::PackageInfo* response)
    {
        try
        {
            auto source = protobuf::decoded<PackageSource>(*request);
            protobuf::encode_shared(
                this->provider->install(source),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::finalize(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->finalize();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext* context,
        const ::cc::signal::Filter* filter,
        ::grpc::ServerWriter<::cc::platform::upgrade::Signal>* writer)
    {
        try
        {
            return this->stream_signals<::cc::platform::upgrade::Signal, SignalQueue>(
                context,
                filter,
                writer);
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

}  // namespace platform::upgrade::grpc
