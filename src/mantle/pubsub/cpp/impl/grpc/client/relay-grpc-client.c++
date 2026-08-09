// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-client.c++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-client.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-relay-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::pubsub::grpc
{
    ReplayPolicyMap Client::get_replay_policies() const
    {
        return cc::protobuf::decoded<ReplayPolicyMap>(
            this->call_check(&Stub::GetReplayPolicies));
    }

    std::optional<ReplayPolicy> Client::get_replay_policy(
        const Topic& topic) const
    {
        pubsub::protobuf::Topics request;
        request.add_topics(topic);

        auto policy_map = cc::protobuf::decoded<ReplayPolicyMap>(
            this->call_check(
                &Stub::GetReplayPolicies,
                request));

        return policy_map.get_opt(topic);
    }

    void Client::assign_replay_policy(
        const std::string& topic,
        const ReplayPolicy& policy)
    {
        pubsub::protobuf::ReplayPolicyMap request;
        auto& map = *request.mutable_map();
        cc::protobuf::encode(policy, &map[topic]);

        this->call_check(
            &Stub::AssignReplayPolicies,
            request);
    }

    void Client::unassign_replay_policy(
        const Topic& topic)
    {
        pubsub::protobuf::Topics request;
        request.add_topics(topic);
        this->call_check(
            &Stub::UnassignReplayPolicies,
            request);
    }

    void Client::clear_replay_policies()
    {
        this->call_check(
            &Stub::UnassignReplayPolicies);
    }

    Snapshot Client::replay_all()
    {
        return this->call_replay({});
    }

    std::optional<Payloads> Client::replay_topic(
        const Topic& topic)
    {
        return this->call_replay({topic}).get_opt(topic);
    }

    Snapshot Client::call_replay(const std::vector<Topic>& topics) const
    {
        pubsub::protobuf::Filters request;
        cc::protobuf::assign_repeated(topics, request.mutable_topics());
        request.set_replay(pubsub::protobuf::ReplayControl::REPLAY_ONLY);

        Snapshot snapshot;
        ::grpc::ClientContext context;
        if (auto reader = this->stub->Subscriber(&context, request))
        {
            pubsub::protobuf::Publication msg;
            while (reader->Read(&msg))
            {
                cc::protobuf::decode(
                    msg.value(),
                    &snapshot[msg.topic()].emplace_back());
            }
        }
        return snapshot;
    }

}  // namespace cc::platform::pubsub::grpc
