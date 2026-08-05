// -*- c++ -*-
//==============================================================================
/// @file broker-relay-control.h++
/// @brief Relay control - broker-side implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-control.h++"
#include "types/shared-ptr-map.h++"
#include "types/valuemap.h++"
#include "types/create-shared.h++"

namespace cc::platform::pubsub
{
    class RelayControl : public ControlInterface,
                         public core::types::enable_create_shared<RelayControl>
    {
        using This = RelayControl;
        using Super = ControlInterface;

        using MappingKey = std::vector<std::string>;

    public:
        using ReplayMap = core::types::ValueMap<MappingKey, core::types::Value>;
        using ReplayMaps = core::types::ValueMap<Topic, ReplayMap>;

    public:
        RelayControl();

        void initialize() override;
        void deinitialize() override;

        ReplayPolicyMap get_replay_policies() const override;
        std::optional<ReplayPolicy> get_replay_policy(
            const Topic &topic) const override;

        void clear_replay_policies() override;

        void assign_replay_policy(
            const std::string& topic,
            const ReplayPolicy &policy) override;

        void unassign_replay_policy(
            const Topic& topic) override;

        Snapshot replay_all() override;
        std::optional<Payloads> replay_topic(
            const Topic& topic) override;

    private:
        std::optional<MappingKey> mapping_key(
            const std::string& topic,
            const core::types::Value& payload);

        void on_message(const std::string& topic,
                        const core::types::Value& payload);

    private:
        std::string signal_handle;
        ReplayPolicyMap policy_map;
        std::mutex replay_map_mtx;
        ReplayMaps replay_maps;
    };

}  // namespace cc::platform::pubsub
