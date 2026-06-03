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

namespace cc::platform::pubsub::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    ::grpc::Status RequestHandler::Subscriber(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::Filters* request,
        ::grpc::ServerWriter<platform::pubsub::protobuf::Publication>* writer)
    {
        pubsub::TopicSet topics(
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::Publisher(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<platform::pubsub::protobuf::Publication>* reader,
        ::google::protobuf::Empty* reply)
    {
        platform::pubsub::protobuf::Publication publication;
        while (reader->Read(&publication))
        {
            pubsub::signal_publication.emit(
                publication.topic(),
                cc::protobuf::decoded<core::types::Value>(publication.value()));
        }

        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::Publish(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::Publication *message,
        ::google::protobuf::Empty* reply)
    {
        try
        {
            pubsub::signal_publication.emit(
                message->topic(),
                cc::protobuf::decoded<core::types::Value>(message->value()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *message, this->peer(context));
        }
    }

}  // namespace cc::platform::pubsub::grpc
