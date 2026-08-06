// -*- c++ -*-
//==============================================================================
/// @file broker-relay-control.c++
/// @brief Relay control - broker-side implementation
/// @author Tor Slettnes
//==============================================================================

#include "broker-relay-control.h++"
#include "platform/symbols.h++"

namespace cc::platform::pubsub
{

    RelayControl::RelayControl()
        : signal_handle(TYPE_NAME_FULL(This))
    {
    }

    void RelayControl::initialize()
    {
        Super::initialize();

        using namespace std::placeholders;
        signal_publication.connect(
            this->signal_handle,
            std::bind(&This::on_message, this, _2, _3));
    }

    void RelayControl::deinitialize()
    {
        signal_publication.disconnect(this->signal_handle);
        Super::deinitialize();
    }

    ReplayPolicyMap RelayControl::get_replay_policies() const
    {
        return this->policy_map;
    }

    std::optional<ReplayPolicy> RelayControl::get_replay_policy(
        const Topic& topic) const
    {
        return this->policy_map.get_opt(topic);
    }

    void RelayControl::clear_replay_policies()
    {
        return this->policy_map.clear();
    }

    void RelayControl::assign_replay_policy(
        const std::string& topic,
        const ReplayPolicy& policy)
    {
        this->policy_map.insert_or_assign(topic, policy);
    }

    void RelayControl::unassign_replay_policy(
        const std::string& topic)
    {
        this->policy_map.erase(topic);
    }

    Snapshot RelayControl::replay_all()
    {
        std::scoped_lock lck(this->replay_map_mtx);
        Snapshot snapshot;
        for (const auto& [topic, replay_map] : this->replay_maps)
        {
            snapshot.insert_or_assign(topic, replay_map.values());
        }
        return snapshot;
    }

    std::optional<Payloads> RelayControl::replay_topic(
        const Topic& topic)
    {
        std::scoped_lock lck(this->replay_map_mtx);
        if (auto* replay_map = this->replay_maps.get_ptr(topic))
        {
            return replay_map->values();
        }
        else
        {
            return {};
        }
    }

    std::optional<RelayControl::MappingKey> RelayControl::mapping_key(
        const std::string& topic,
        const core::types::Value& payload)
    {
        if (auto* policy = this->policy_map.get_ptr(topic))
        {
            if (policy->replay_latest)
            {
                MappingKey mapping_key;
                mapping_key.reserve(policy->key_paths.size());
                for (const std::vector<std::string>& key_path : policy->key_paths)
                {
                    mapping_key.push_back(payload.get_nested(key_path).as_string());
                }
                return mapping_key;
            }
        }
        return {};
    }

    void RelayControl::on_message(const std::string& topic,
                                  const core::types::Value& payload)
    {
        if (auto mapping_key = this->mapping_key(topic, payload))
        {
            std::scoped_lock lck(this->replay_map_mtx);
            this->replay_maps[topic][*mapping_key] = payload;
        }
    }

}  // namespace cc::platform::pubsub
