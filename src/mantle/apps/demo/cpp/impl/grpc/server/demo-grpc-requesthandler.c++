// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.c++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-requesthandler.h++"
#include "demo-grpc-signalqueue.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-inline.h++"
#include "protobuf-message.h++"
#include "status/exceptions.h++"

namespace demo::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& api_provider)
        : Super(),
          provider(api_provider)
    {
        logf_debug("Demo gRPC RequestHandler Constructor");
    }

    ::grpc::Status RequestHandler::say_hello(
        ::grpc::ServerContext* context,
        const ::CC::Demo::Greeting* request,
        ::google::protobuf::Empty* response)
    {
        // We received a greeting from a client.  We decode and pass this on to
        // our `api_provider`. Normally this is the native C++ implementation,
        // which will then emit a signal to registered callbacks (slots). (These
        // include gRPC client proxies that are streaming back greetings via the
        // `watch()` method, as well as available interfaces for other messaging
        // platforms.)

        try
        {
            this->provider->say_hello(protobuf::decoded<Greeting>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_current_time(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::CC::Demo::TimeData* response)
    {
        try
        {
            protobuf::encode(this->provider->get_current_time(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::start_ticking(
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

    ::grpc::Status RequestHandler::stop_ticking(
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

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext* context,
        const CC::Signal::Filter* request,
        ::grpc::ServerWriter<CC::Demo::Signal>* writer)
    {
        return this->stream_signals<CC::Demo::Signal, SignalQueue>(context, request, writer);
    }

}  // namespace demo::grpc
