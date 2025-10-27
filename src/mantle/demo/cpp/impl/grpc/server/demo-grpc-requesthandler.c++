// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.c++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-requesthandler.h++"
#include "demo-grpc-signalqueue.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"
#include "status/exceptions.h++"

namespace demo::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& api_provider)
        : Super(),
          provider(api_provider)
    {
        logf_debug("Demo gRPC RequestHandler Constructor");
    }

    ::grpc::Status RequestHandler::SayHello(
        ::grpc::ServerContext* context,
        const ::cc::demo::protobuf::Greeting* request,
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
            this->provider->say_hello(::protobuf::decoded<Greeting>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetCurrentTime(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::demo::protobuf::TimeData* response)
    {
        try
        {
            ::protobuf::encode(this->provider->get_current_time(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
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
            return this->failure(std::current_exception(), *request, context->peer());
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
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext* context,
        const cc::protobuf::signal::Filter* request,
        ::grpc::ServerWriter<cc::demo::protobuf::Signal>* writer)
    {
        return this->stream_signals<cc::demo::protobuf::Signal, SignalQueue>(context, request, writer);
    }

}  // namespace demo::grpc
