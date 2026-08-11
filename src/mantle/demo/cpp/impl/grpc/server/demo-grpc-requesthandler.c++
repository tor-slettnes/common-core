// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.c++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes
//==============================================================================

#include "demo-grpc-requesthandler.h++"
#include "demo-grpc-signalqueue.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-message.h++"
#include "protobuf-version.h++"
#include "protobuf-inline.h++"
#include "platform/path.h++"
#include "status/exceptions.h++"

namespace cc::demo::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& api_provider)
        : Super(),
          provider(api_provider)
    {
        logf_debug("Demo gRPC RequestHandler Constructor");
    }

    ::grpc::Status RequestHandler::ServiceCheck(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ServiceCheckResponse* response)
    {
        response->set_api_level(APILEVEL_CURRENT);
        response->set_server_name(core::platform::path->exec_name());
        cc::protobuf::populate_version(response->mutable_server_version());
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::SayHello(
        ::grpc::ServerContext* context,
        const demo::protobuf::Greeting* request,
        ::google::protobuf::Empty* response)
    {
        // We received a greeting from a client.  We decode and pass this on to
        // our `api_provider`. Normally this is the host-native implementation,
        // which will then emit a signal to registered callbacks (slots). (These
        // include gRPC client proxies that are streaming back greetings via the
        // `watch()` method, as well as available interfaces for other messaging
        // platforms.)

        try
        {
            this->provider->say_hello(cc::protobuf::decoded<Greeting>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetCurrentTime(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        demo::protobuf::TimeData* response)
    {
        try
        {
            cc::protobuf::encode(this->provider->get_current_time(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::StartTicking(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->start_ticking();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::StopTicking(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->stop_ticking();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext* context,
        const cc::protobuf::signal::Filter* request,
        ::grpc::ServerWriter<cc::demo::protobuf::Signal>* writer)
    {
        return this->stream_signals<cc::demo::protobuf::Signal, SignalQueue>(context, request, writer);
    }

}  // namespace cc::demo::grpc
