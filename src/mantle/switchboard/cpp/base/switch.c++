// -*- c++ -*-
//==============================================================================
/// @file switch.c++
/// @brief Switch Base class
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switch.h++"
#include "switchboard-provider.h++"
#include "status/exceptions.h++"

namespace switchboard
{
    //==========================================================================
    /// @class Switch

    Switch::Switch(const SwitchName &name,
                   const std::shared_ptr<switchboard::Provider> &provider)
        : name_(name),
          provider_(provider),
          spec_ref(std::make_shared<Specification>()),
          status_ref(std::make_shared<Status>())
    {
        logf_debug("Created switch %r", this->name());
    }

    void Switch::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                            "Switch(%r, primary=%b, dependencies=%s, interceptors=%s, "
                            "state=%s, active=%b",
                            this->name(),
                            this->primary(),
                            this->dependencies(),
                            this->interceptors(),
                            this->state(),
                            this->active());

        if (core::status::Error::ptr error = this->error())
        {
            stream << ", error=" << *error;
        }

        stream << ")";
    }

    const SwitchName &Switch::name() const noexcept
    {
        return this->name_;
    }

    std::shared_ptr<Provider> Switch::provider() const
    {
        if (std::shared_ptr<Provider> ref = this->provider_.lock())
        {
            return ref;
        }
        else
        {
            return {};
        }
    }

    const DependencyMap &Switch::dependencies() const noexcept
    {
        return this->spec()->dependencies;
    }

    DependencyRef Switch::get_dependency(const SwitchName &switch_name) const noexcept
    {
        try
        {
            return this->dependencies().at(switch_name);
        }
        catch (const std::out_of_range &)
        {
            return {};
        }
    }

    SwitchSet Switch::get_predecessors() const noexcept
    {
        SwitchSet set;
        for (const auto &[name, dep] : this->dependencies())
        {
            if (const SwitchRef &predecessor = dep->predecessor())
            {
                set.insert(predecessor);
            }
        }
        return set;
    }

    SwitchSet Switch::get_successors() const noexcept
    {
        SwitchSet set;
        for (const auto &[name, sw] : this->provider()->get_switches())
        {
            if (sw->dependencies().count(this->name()))
            {
                set.insert(sw);
            }
        }
        return set;
    }

    SwitchSet Switch::get_ancestors() const noexcept
    {
        SwitchSet set;
        for (const SwitchRef &sw : this->get_predecessors())
        {
            if (set.count(sw) == 0)
            {
                set.insert(sw);
                set.merge(sw->get_ancestors());
            }
        }
        return set;
    }

    SwitchSet Switch::get_descendents() const noexcept
    {
        SwitchSet set;
        for (const SwitchRef &sw : this->get_successors())
        {
            if (set.count(sw) == 0)
            {
                set.insert(sw);
                set.merge(sw->get_descendents());
            }
        }
        return set;
    }

    const InterceptorMap &Switch::interceptors() const noexcept
    {
        return this->spec()->interceptors;
    }

    InterceptorRef Switch::get_interceptor(
        const InterceptorName &name,
        bool required) const
    {
        try
        {
            return this->spec()->interceptors.at(name);
        }
        catch (const std::out_of_range &)
        {
            if (required)
            {
                throw core::exception::NotFound("Interceptor not found", name);
            }
            else
            {
                return {};
            }
        }
    }

    SpecRef Switch::spec() const
    {
        return this->spec_ref;
    }

    void Switch::set_spec(
        const Specification &spec)
    {
        *this->spec_ref = spec;
    }

    void Switch::set_primary(bool primary)
    {
        this->update_spec(
            primary, // primary
            {},      // descriptions
            false,   // replace_descriptions
            {},      // dependencies
            false,   // replace_dependencies
            {},      // interceptors
            false,   // replace_interceptors
            false);  // update_descendents
    }

    bool Switch::primary() const noexcept
    {
        return this->spec()->primary;
    }

    void Switch::set_descriptions(const DescriptionMap &descriptions)
    {
        this->update_spec(
            {},            // primary
            descriptions,  // description
            false,         // replace_descriptions
            {},            // dependencies
            false,         // replace_dependencies
            {},            // interceptors
            false,         // replace_interceptors
            false);        // propagate
    }

    DescriptionMap Switch::descriptions() const noexcept
    {
        return this->spec()->descriptions;
    }

    std::optional<std::string> Switch::description(const LanguageCode &language) const noexcept
    {
        if (const auto desc = this->spec()->descriptions.get_opt(language))
        {
            return desc->text;
        }
        else
        {
            return {};
        }
    }

    std::string Switch::target_text(bool target,
                                    const std::string &lang) const noexcept
    {
        try
        {
            return this->spec()->descriptions.at(lang).target_texts.at(target);
        }
        catch (const std::out_of_range &)
        {
            return "";
        }
    }

    std::string Switch::state_text(State state,
                                   const std::string &lang) const noexcept
    {
        try
        {
            return this->spec()->descriptions.at(lang).state_texts.at(state);
        }
        catch (const std::out_of_range &)
        {
            return "";
        }
    }

    StatusRef Switch::status() const
    {
        return this->status_ref;
    }

    Switch::operator bool() const noexcept
    {
        return this->active();
    }

    bool Switch::active() const noexcept
    {
        return this->status()->active;
    }

    bool Switch::failed() const noexcept
    {
        return this->settled_state() == STATE_FAILED;
    }

    bool Switch::is_active_target(State state) noexcept
    {
        return Switch::target_state(state) == STATE_ACTIVE;
    }

    bool Switch::is_settled(State state)
    {
        switch (state)
        {
        case STATE_INACTIVE:
        case STATE_ACTIVE:
        case STATE_FAILED:
            return true;

        default:
            return false;
        }
    }

    bool Switch::settled() const noexcept
    {
        return Switch::is_settled(this->state());
    }

    bool Switch::pending() const noexcept
    {
        return !this->settled();
    }

    State Switch::state() const noexcept
    {
        return this->status()->current_state;
    }

    State Switch::settled_state() const noexcept
    {
        return this->status()->settled_state;
    }

    State Switch::target_state() const noexcept
    {
        return Switch::target_state(this->state());
    }

    State Switch::target_state(State current) noexcept
    {
        switch (current)
        {
        case STATE_ACTIVATING:
            return STATE_ACTIVE;

        case STATE_DEACTIVATING:
            return STATE_INACTIVE;

        case STATE_FAILING:
            return STATE_FAILED;

        default:
            return current;
        }
    }

    State Switch::target_state(bool active) noexcept
    {
        return active ? STATE_ACTIVE : STATE_INACTIVE;
    }

    State Switch::auto_state() const noexcept
    {
        enum SatisfactionLevel
        {
            NEUTRAL = 0x00,
            PENDING = 0x01,
            WEAK = 0x02,
            STRONG = 0x04,
        };

        uint32_t satisfied = NEUTRAL;
        uint32_t unsatisfied = NEUTRAL;
        uint32_t failed = NEUTRAL;
        uint32_t undetermined = NEUTRAL;

        auto msg = standard_log_message(core::status::Level::TRACE);
        msg->format("Switch [%r]: Determining auto state:\n", this->name());

        for (const auto &[name, dep] : this->dependencies())
        {
            switch (dep->derived_state(this->active()))
            {
            case STATE_ACTIVATING:
                satisfied |= PENDING;
                break;

            case STATE_ACTIVE:
                satisfied |= (dep->sufficient() ? STRONG : WEAK);
                break;

            case STATE_DEACTIVATING:
                unsatisfied |= PENDING;
                break;

            case STATE_INACTIVE:
                unsatisfied |= (dep->sufficient() ? WEAK : STRONG);
                break;

            case STATE_FAILING:
                failed = PENDING;
                break;

            case STATE_FAILED:
                failed = STRONG;
                break;

            default:
                undetermined |= (dep->sufficient() ? WEAK : STRONG);
                break;
            }

            msg->format("     %r: satisfied=%02X, unsatisified=%02X, failed=%02X, undetermined=%02X\n",
                        *dep,
                        satisfied,
                        unsatisfied,
                        failed,
                        undetermined);
        }

        State state = failed & STRONG         ? STATE_FAILED
                      : satisfied & STRONG    ? STATE_ACTIVE
                      : unsatisfied & STRONG  ? STATE_INACTIVE
                      : undetermined & STRONG ? STATE_UNSET
                      : failed & PENDING      ? STATE_FAILING
                      : satisfied & PENDING   ? STATE_ACTIVATING
                      : unsatisfied & PENDING ? STATE_DEACTIVATING
                      : satisfied & WEAK      ? STATE_ACTIVE
                      : unsatisfied & WEAK    ? STATE_INACTIVE
                      : this->settled()       ? this->state()
                      : undetermined & WEAK   ? STATE_UNSET
                      : this->active()        ? STATE_ACTIVE
                                              : STATE_INACTIVE;
        msg->format(" --> auto_state=%s", state);
        msg->dispatch();
        return state;
    }

    bool Switch::set_active(
        bool active,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
        bool trigger_descendents,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        return this->set_target(
            this->target_state(active),
            {},
            attributes,
            clear_existing,
            invoke_interceptors,
            trigger_descendents,
            reevaluate,
            on_cancel,
            on_error);
    }

    bool Switch::set_error(
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
        bool trigger_descendents,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        return this->set_target(
            error ? STATE_FAILED : this->target_state(this->active()),
            error,
            attributes,
            clear_existing,
            invoke_interceptors,
            trigger_descendents,
            reevaluate,
            on_cancel,
            on_error);
    }

    bool Switch::set_auto(
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
        bool trigger_descendents,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        return this->set_target(
            STATE_UNSET,
            {},
            attributes,
            clear_existing,
            invoke_interceptors,
            trigger_descendents,
            reevaluate,
            on_cancel,
            on_error);
    }

    core::status::Error::ptr Switch::error() const noexcept
    {
        return this->status()->error;
    }

    ErrorMap Switch::errors() const noexcept
    {
        ErrorMap errors;
        if (this->failed())
        {
            if (this->error())
            {
                errors.insert_or_assign(
                    std::const_pointer_cast<Switch>(this->shared_from_this()),
                    this->error());
            }

            for (const SwitchRef &pred : this->get_predecessors())
            {
                errors.merge(pred->errors());
            }
        }
        return errors;
    }

    /// Return this switch's status
    const core::types::KeyValueMap &Switch::attributes() const noexcept
    {
        return this->status()->attributes;
    }

    core::types::Value Switch::attribute(const std::string &key) const
    {
        return this->status()->attributes.get(key);
    }

    void Switch::set_attribute(
        const std::string &name,
        const core::types::Value &value)
    {
        this->set_attributes({{name, value}});
    }

    CulpritsMap Switch::culprits(bool expected) const noexcept
    {
        CulpritsMap culprits;
        State current_state = this->state();
        State expected_state = this->target_state(expected);

        if (current_state != expected_state)
        {
            if (!this->primary())
            {
                for (const auto &[name, dep] : this->dependencies())
                {
                    if (auto pred_expected_state = dep->expected_state())
                    {
                        if (*pred_expected_state != expected_state)
                        {
                            if (auto pred_expected_value = dep->expected_predecessor_value(expected))
                            {
                                if (const SwitchRef &pred = dep->predecessor())
                                {
                                    culprits.merge(pred->culprits(pred_expected_value.value()));
                                }
                            }
                        }
                    }
                    else if (dep->sufficient())
                    {
                        culprits.clear();
                        break;
                    }
                }
            }

            if (culprits.empty())
            {
                culprits.insert_or_assign(
                    std::const_pointer_cast<Switch>(this->shared_from_this()),
                    current_state);
            }
        }

        return culprits;
    }

}  // namespace switchboard
