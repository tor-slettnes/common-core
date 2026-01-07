// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-requesthandler.c++
/// @brief Forward message publications over gRPC streams
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-requesthandler.h++"
#include "relay-grpc-messagequeue.h++"
#include "relay-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace pubsub::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    ::grpc::Status RequestHandler::Subscriber(
        ::grpc::ServerContext* context,
        const ::cc::platform::pubsub::protobuf::Filters* request,
        ::grpc::ServerWriter<::cc::platform::pubsub::protobuf::Publication>* writer)
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

    ::grpc::Status RequestHandler::Publisher(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<::cc::platform::pubsub::protobuf::Publication>* reader,
        ::google::protobuf::Empty* reply)
    {
        ::cc::platform::pubsub::protobuf::Publication publication;
        while (reader->Read(&publication))
        {
            pubsub::signal_publication.emit(
                publication.topic(),
                protobuf::decoded<core::types::Value>(publication.payload()));
        }

        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::Publish(
        ::grpc::ServerContext* context,
        const ::cc::platform::pubsub::protobuf::Publication *message,
        ::google::protobuf::Empty* reply)
    {
        try
        {
            pubsub::signal_publication.emit(
                message->topic(),
                protobuf::decoded<core::types::Value>(message->payload()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *message, context->peer());
        }
    }

}  // namespace pubsub::grpc
