// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-requesthandler.c++
/// @brief Handle Upgrade gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-requesthandler.h++"
#include "upgrade-grpc-signalqueue.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace upgrade::grpc
{
    RequestHandler::RequestHandler()
        : provider(upgrade::upgrade.get_shared())
    {
    }

    RequestHandler::RequestHandler(
        const std::shared_ptr<upgrade::ProviderInterface> &provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::scan(
        ::grpc::ServerContext *context,
        const ::cc::platform::upgrade::PackageSource *request,
        ::cc::platform::upgrade::PackageCatalogue *response)
    {
        try
        {
            protobuf::encode(
                this->provider->scan(protobuf::decoded<PackageSource>(*request)),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_sources(
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        ::cc::platform::upgrade::PackageSources *response)
    {
        try
        {
            protobuf::encode(this->provider->list_sources(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_available(
        ::grpc::ServerContext *context,
        const ::cc::platform::upgrade::PackageSource *request,
        ::cc::platform::upgrade::PackageCatalogue *response)
    {
        try
        {
            protobuf::encode(
                this->provider->list_available(protobuf::decoded<PackageSource>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::best_available(
        ::grpc::ServerContext *context,
        const ::cc::platform::upgrade::PackageSource *request,
        ::cc::platform::upgrade::PackageInfo *response)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->best_available(protobuf::decoded<PackageSource>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), context->peer());
        }
    }

    ::grpc::Status RequestHandler::install(
        ::grpc::ServerContext *context,
        const ::cc::platform::upgrade::InstallRequest *request,
        ::cc::platform::upgrade::PackageInfo *response)
    {
        try
        {
            auto source = protobuf::decoded<PackageSource>(request->source());
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
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        ::google::protobuf::Empty *response)
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
        ::grpc::ServerContext *context,
        const ::cc::signal::Filter *filter,
        ::grpc::ServerWriter<::cc::platform::upgrade::Signal> *writer)
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

} // namespace upgrade::grpc
