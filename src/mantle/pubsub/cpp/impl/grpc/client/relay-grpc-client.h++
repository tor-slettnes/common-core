// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-client.h++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-grpc-base-client.h++"
#include "types/create-shared.h++"

namespace cc::platform::pubsub::grpc
{
    //--------------------------------------------------------------------------
    // Client -- Final

    class Client : public ClientImpl,
                   public ControlInterface,
                   public core::types::enable_create_shared<Client>
    {
    protected:
        using ClientImpl::ClientImpl;

    public:
        ReplayPolicyMap get_replay_policies() const override;
        std::optional<ReplayPolicy> get_replay_policy(
            const Topic& topic) const override;

        void clear_replay_policies() override;

        void assign_replay_policy(
            const std::string& topic,
            const ReplayPolicy& policy) override;

        void unassign_replay_policy(
            const Topic& topic) override;

        Snapshot replay_all() override;
        std::optional<Payloads> replay_topic(
            const Topic& topic) override;

    private:
        Snapshot call_replay(const std::vector<Topic>& topics) const;
    };

}  // namespace cc::platform::pubsub::grpc
