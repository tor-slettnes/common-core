// -*- c++ -*-
//==============================================================================
/// @file switch-dependency.c++
/// @brief Upstream dependency between a switch and its predecessor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switch-dependency.h++"
#include "switchboard-provider.h++"

namespace switchboard
{
    //==========================================================================
    /// @class Dependency

    Dependency::Dependency(
        const std::weak_ptr<Provider> &provider,
        const SwitchName &predecessor_name,
        StateMask trigger_states,
        DependencyPolarity polarity,
        bool hard,
        bool sufficient)
        : provider_(provider),
          predecessor_name_(predecessor_name),
          trigger_states_(trigger_states),
          polarity_(polarity),
          hard_(hard),
          sufficient_(sufficient)
    {
    }

    ProviderRef Dependency::provider() const
    {
        return this->provider_.lock();
    }

    SwitchRef Dependency::predecessor() const
    {
        if (auto provider = this->provider())
        {
            return provider->get_switch(this->predecessor_name());
        }
        else
        {
            return {};
        }
    }

    const SwitchName &Dependency::predecessor_name() const
    {
        return this->predecessor_name_;
    }

    State Dependency::predecessor_state() const
    {
        if (auto pred = this->predecessor())
        {
            return pred->state();
        }
        else
        {
            return STATE_UNSET;
        }
    }

    StateMask Dependency::trigger_states() const
    {
        return this->trigger_states_;
    }

    DependencyPolarity Dependency::polarity() const
    {
        return this->polarity_;
    }

    bool Dependency::hard() const
    {
        return this->hard_;
    }

    bool Dependency::sufficient() const
    {
        return this->sufficient_;
    }

    bool Dependency::auto_trigger(State pred_state) const
    {
        return (this->trigger_states() & static_cast<StateMask>(pred_state)) != 0;
    }

    State Dependency::derived_state(bool active) const
    {
        if (this->polarity() == DependencyPolarity::TOGGLE)
        {
            if (this->auto_trigger(this->predecessor_state()))
            {
                active = !active;
            }
            return active ? STATE_ACTIVE : STATE_INACTIVE;
        }
        else
        {
            return this->expected_state().value();
        }
    }

    std::optional<State> Dependency::expected_state() const
    {
        switch (this->polarity())
        {
        case DependencyPolarity::POSITIVE:
            return this->predecessor_state();

        case DependencyPolarity::NEGATIVE:
            return this->inverted(this->predecessor_state());

        case DependencyPolarity::TOGGLE:
            return {};

        default:
            // We should never get here; but if we do, flag this as an unsatisfied dependency
            return STATE_UNSET;
        }
    }

    std::optional<bool> Dependency::expected_predecessor_value(bool expected_successor_value) const
    {
        switch (this->polarity())
        {
        case DependencyPolarity::POSITIVE:
            return expected_successor_value;

        case DependencyPolarity::NEGATIVE:
            return !expected_successor_value;

        case DependencyPolarity::TOGGLE:
            return {};

        default:
            return {};
        }
    }

    State Dependency::inverted(State state)
    {
        switch (state)
        {
        case STATE_DEACTIVATING:
            return STATE_ACTIVATING;

        case STATE_INACTIVE:
            return STATE_ACTIVE;

        case STATE_ACTIVATING:
            return STATE_DEACTIVATING;

        case STATE_ACTIVE:
            return STATE_INACTIVE;

        default:
            return state;
        }
    }

    void Dependency::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"predecessor", this->predecessor_name()},
            {"trigger_states", core::types::ValueList::create_from(state_set(this->trigger_states()))},
            {"polarity", core::str::convert_from(this->polarity())},
            {"hard", this->hard()},
            {"sufficient", this->sufficient()},
        });
    }

}  // namespace switchboard
