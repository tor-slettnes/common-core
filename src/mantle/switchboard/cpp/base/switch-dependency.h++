// -*- c++ -*-
//==============================================================================
/// @file switch-dependency.h++
/// @brief Upstream dependency between a switch and its predecessor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard-types.h++"

#include "types/streamable.h++"
#include "types/create-shared.h++"

#include <memory>

namespace switchboard
{
    //==========================================================================
    // @class Dependency

    class Dependency : public core::types::Streamable,
                       public core::types::enable_create_shared<Dependency>
    {
    public:
        // static constexpr StateMask DEFAULT_TRIGGERS = STATE_SETTLED;
        static constexpr StateMask DEFAULT_TRIGGERS = 0;

    protected:
        Dependency(
            const std::weak_ptr<Provider> &provider,
            const SwitchName &predecessor_name,
            StateMask trigger_states = DEFAULT_TRIGGERS,
            DependencyPolarity polarity = DependencyPolarity::POSITIVE,
            bool hard = false,
            bool sufficient = false);

    public:
        // Accessor methods
        ProviderRef provider() const;
        SwitchRef predecessor() const;
        const SwitchName &predecessor_name() const;
        State predecessor_state() const;

        StateMask trigger_states() const;
        DependencyPolarity polarity() const;
        bool hard() const;
        bool sufficient() const;

    public:
        bool auto_trigger(State pred_state) const;
        State derived_state(bool active) const;
        std::optional<State> expected_state() const;
        std::optional<bool> expected_predecessor_value(bool expected_successor_value) const;
        static State inverted(State state);

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        std::weak_ptr<Provider> provider_;
        SwitchName predecessor_name_;
        StateMask trigger_states_;
        DependencyPolarity polarity_;
        bool hard_;
        bool sufficient_;
    };
}  // namespace switchboard
