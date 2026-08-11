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
#include "protobuf-relay-types.h++"
#include "protobuf-version.h++"
#include "platform/path.h++"
#include "protobuf-inline.h++"

namespace cc::platform::pubsub::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    RequestHandler::RequestHandler(
        const std::shared_ptr<ControlInterface> relay_control)
        : relay_control(relay_control)
    {
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

    ::grpc::Status RequestHandler::AssignReplayPolicies(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::ReplayPolicyMap* request,
        ::google::protobuf::Empty* reply)
    {
        auto policy_map = cc::protobuf::decoded<ReplayPolicyMap>(*request);
        for (const auto& [topic, policy] : policy_map)
        {
            this->relay_control->assign_replay_policy(topic, policy);
        }
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::UnassignReplayPolicies(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::Topics* request,
        ::google::protobuf::Empty* reply)
    {
        if (request->topics().empty())
        {
            this->relay_control->clear_replay_policies();
        }
        else
        {
            for (const Topic& topic : request->topics())
            {
                this->relay_control->unassign_replay_policy(topic);
            }
        }
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::GetReplayPolicies(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::Topics* request,
        platform::pubsub::protobuf::ReplayPolicyMap* reply)
    {
        if (request->topics().empty())
        {
            cc::protobuf::encode(
                this->relay_control->get_replay_policies(),
                reply);
        }
        else
        {
            auto& reply_map = *reply->mutable_map();
            for (const Topic& topic : request->topics())
            {
                if (const auto& policy = this->relay_control->get_replay_policy(topic))
                {
                    cc::protobuf::encode(*policy, &reply_map[topic]);
                }
            }
        }

        return ::grpc::Status::OK;
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
        const platform::pubsub::protobuf::Publication* message,
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

    ::grpc::Status RequestHandler::Subscriber(
        ::grpc::ServerContext* context,
        const platform::pubsub::protobuf::Filters* request,
        ::grpc::ServerWriter<platform::pubsub::protobuf::Publication>* writer)
    {
        pubsub::TopicSet topics(
            request->topics().begin(),
            request->topics().end());

        MessageQueue queue(topics);

        try
        {
            if (request->replay() != pubsub::protobuf::ReplayControl::REPLAY_OFF)
            {
                for (const auto& [topic, payloads] : this->relay_control->replay_all())
                {
                    for (const core::types::Value& payload : payloads)
                    {
                        queue.enqueue_message(topic, payload);
                    }
                }
            }

            if (request->replay() == pubsub::protobuf::ReplayControl::REPLAY_ONLY)
            {
                queue.close();
            }
            else
            {
                queue.initialize();
            }

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

}  // namespace cc::platform::pubsub::grpc
