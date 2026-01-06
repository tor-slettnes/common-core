// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-requesthandler.c++
/// @brief Message forwarding over gRPC streams
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-requesthandler.h++"
#include "relay-grpc-messagequeue.h++"
#include "relay-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace relay::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    ::grpc::Status RequestHandler::Reader(
        ::grpc::ServerContext* context,
        const ::cc::platform::relay::protobuf::Filters* request,
        ::grpc::ServerWriter<::cc::platform::relay::protobuf::Message>* writer)
    {
        std::unordered_set<std::string> topics(
            request->topics().begin(),
            request->topics().end());

        MessageQueue queue(topics);
        queue.initialize();

        try
        {
            queue.stream(context, writer);
            queue.deinitialize();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            queue.deinitialize();
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::Writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<::cc::platform::relay::protobuf::Message>* reader,
        ::google::protobuf::Empty* reply)
    {
        ::cc::platform::relay::protobuf::Message message;
        while (reader->Read(&message))
        {
            relay::signal_message.emit(
                message.topic(),
                protobuf::decoded<core::types::Value>(message.payload()));
        }

        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::Publish(
        ::grpc::ServerContext* context,
        const ::cc::platform::relay::protobuf::Message* message,
        ::google::protobuf::Empty* reply)
    {
        try
        {
            relay::signal_message.emit(
                message->topic(),
                protobuf::decoded<core::types::Value>(message->payload()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *message, context->peer());
        }
    }

}  // namespace relay::grpc
