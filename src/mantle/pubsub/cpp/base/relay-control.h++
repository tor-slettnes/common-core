// -*- c++ -*-
//==============================================================================
/// @file relay-control.h++
/// @brief Relay control abstract base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "types/valuemap.h++"
#include "thread/blockingqueue.h++"

namespace cc::platform::pubsub
{
    //--------------------------------------------------------------------------
    // Abstract Interface

    class ControlInterface
    {
        using This = ControlInterface;

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

    public:
        virtual ReplayPolicyMap get_replay_policies() const = 0;
        virtual std::optional<ReplayPolicy> get_replay_policy(
            const Topic& topic) const = 0;

        virtual void assign_replay_policy(
            const Topic& topic,
            const ReplayPolicy& policy) = 0;

        virtual void unassign_replay_policy(
            const Topic& topic) = 0;

        virtual void clear_replay_policies() = 0;

        virtual Snapshot replay_all() = 0;
        virtual std::optional<Payloads> replay_topic(
            const Topic& topic) = 0;
    };

}  // namespace cc::platform::pubsub
