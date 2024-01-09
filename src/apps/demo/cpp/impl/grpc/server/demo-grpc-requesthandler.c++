// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.c++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-requesthandler.h++"
#include "translate-protobuf-demo.h++"
#include "translate-protobuf-inline.h++"
#include "protobuf-message.h++"

namespace cc::demo::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& api_provider)
        : provider(api_provider)
    {
        logf_debug("Demo gRPC RequestHandler Constructor");
    }

    ::grpc::Status RequestHandler::say_hello(
        ::grpc::ServerContext* context,
        const ::CC::Demo::Greeting* request,
        ::google::protobuf::Empty* response)
    {
        // We received a greeting from a client.  Emit a signal to registered
        // callbacks (slots). (This includes `Publisher::on_signal_greeting()`,
        // which then forwards the signals to clients as a published message.)
        this->provider->say_hello(protobuf::decoded<Greeting>(*request));
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::get_current_time(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::CC::Demo::TimeData* response)
    {
        protobuf::encode(this->provider->get_current_time(), response);
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::start_ticking(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        this->provider->start_ticking();
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::stop_ticking(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        this->provider->stop_ticking();
        return ::grpc::Status::OK;
    }

}  // namespace cc::demo::grpc
