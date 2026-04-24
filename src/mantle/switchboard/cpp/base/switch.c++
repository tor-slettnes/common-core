// -*- c++ -*-
//==============================================================================
/// @file switch.c++
/// @brief Switch Base class
/// @author Tor Slettnes
//==============================================================================

#include "switch.h++"
#include "switchboard-provider.h++"
#include "status/exceptions.h++"
#include "string/convert.h++"

namespace switchboard
{
    bool operator==(const Switch &lhs, const Switch &rhs)
    {
        return ((lhs.name() == rhs.name()) &&
                (lhs.spec() == rhs.spec()) &&
                (lhs.status() == rhs.status()));
    }

    bool operator!=(const Switch &lhs, const Switch &rhs)
    {
        return !(lhs == rhs);
    }

    //==========================================================================
    /// @class Switch

    Switch::Switch(const SwitchName &name,
                   const std::shared_ptr<switchboard::Provider> &provider)
        : name_(name),
          provider_(provider),
          spec_ref(std::make_shared<Specification>()),
          status_ref(std::make_shared<Status>())
    {
    }

    void Switch::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->emplace_back("name", this->name());
        this->spec()->to_tvlist(tvlist);
        this->status()->to_tvlist(tvlist);
    }

    void Switch::to_stream(std::ostream &ostream) const
    {
        ostream << "Switch("
                << std::quoted(this->name())
                << ", "
                << this->state()
                << ")";
    }

    SwitchAliases Switch::names() const noexcept
    {
        SwitchAliases aliases = {this->name()};
        aliases.insert(this->spec()->aliases.begin(),
                       this->spec()->aliases.end());
        return aliases;
    }

    const SwitchName &Switch::name() const noexcept
    {
        return this->name_;
    }

    const SwitchAliases &Switch::aliases() const noexcept
    {
        return this->spec()->aliases;
    }

    // Add new aliases for this witch
    void Switch::set_aliases(const SwitchAliases &aliases)
    {
        this->update_spec(
            {},       // primary
            aliases,  // aliases
            true,     // replace_aliases
            {},       // localizations
            false,    // replace_localizations
            {},       // dependencies
            false,    // replace_dependencies
            {},       // interceptors
            false,    // replace_interceptors
            false);   // update_state
    }

    // Add new aliases for this witch
    void Switch::add_aliases(const SwitchAliases &aliases)
    {
        this->update_spec(
            {},       // primary
            aliases,  // aliases
            false,    // replace_aliases
            {},       // localizations
            false,    // replace_localizations
            {},       // dependencies
            false,    // replace_dependencies
            {},       // interceptors
            false,    // replace_interceptors
            false);   // update_state
    }

    // Remove one or more aliases for this switch
    void Switch::remove_aliases(const SwitchAliases &aliases)
    {
        SwitchAliases updated_set = this->aliases();

        for (const SwitchName &removal : aliases)
        {
            updated_set.erase(removal);
        }

        this->update_spec(
            {},           // primary
            updated_set,  // aliases
            true,         // replace_aliases
            {},           // localizations
            false,        // replace_localizations
            {},           // dependencies
            false,        // replace_dependencies
            {},           // interceptors
            false,        // replace_interceptors
            false);       // update_state
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

    bool Switch::independent() const noexcept
    {
        return this->dependencies().empty();
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

    SwitchSet Switch::get_descendants() const noexcept
    {
        SwitchSet set;
        for (const SwitchRef &sw : this->get_successors())
        {
            if (set.count(sw) == 0)
            {
                set.insert(sw);
                set.merge(sw->get_descendants());
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
            primary,  // primary
            {},       // aliases
            false,    // replace_aliases
            {},       // localizations
            false,    // replace_localizations
            {},       // dependencies
            false,    // replace_dependencies
            {},       // interceptors
            false,    // replace_interceptors
            false);   // update_descendants
    }

    bool Switch::primary() const noexcept
    {
        return this->spec()->primary;
    }

    void Switch::set_localizations(const LocalizationMap &localizations)
    {
        this->update_spec(
            {},             // primary
            {},             // aliases
            false,          // replace_aliases
            localizations,  // localization
            false,          // replace_localizations
            {},             // dependencies
            false,          // replace_dependencies
            {},             // interceptors
            false,          // replace_interceptors
            false);         // propagate
    }

    LocalizationMap Switch::localizations() const noexcept
    {
        return this->spec()->localizations;
    }

    std::optional<Localization> Switch::localization(
        const LanguageCode &language) const noexcept
    {
        return this->spec()->localizations.get_opt(language);
    }

    std::optional<std::string> Switch::description(const LanguageCode &language) const noexcept
    {
        if (const auto &localized = this->localization(language))
        {
            return localized->description;
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> Switch::activate_text(
        const std::string &lang) const noexcept
    {
        if (const auto &localized = this->localization(lang))
        {
            return localized->activate_text;
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> Switch::deactivate_text(
        const std::string &lang) const noexcept
    {
        if (const auto &localized = this->localization(lang))
        {
            return localized->deactivate_text;
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> Switch::state_text(
        const std::string &lang) const noexcept
    {
        if (const auto &localized = this->localization(lang))
        {
            return localized->state_texts.try_to_string(this->state());
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> Switch::state_text(
        State state,
        const std::string &lang) const noexcept
    {
        if (const auto &localized = this->localization(lang))
        {
            return localized->state_texts.try_to_string(state);
        }
        else
        {
            return {};
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

    bool Switch::settled() const noexcept
    {
        return is_settled(this->state());
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
        msg->format(
            "Switch [%s]: Determining auto state:\n",
            this->name());

        State current_state = this->state();

        for (const auto &[name, dep] : this->dependencies())
        {
            std::string vote;

            if (auto derived_state = dep->derived_state(current_state))
            {
                switch (derived_state.value())
                {
                case STATE_ACTIVATING:
                    satisfied |= PENDING;
                    vote = "pending satisfaction";
                    break;

                case STATE_ACTIVE:
                    satisfied |= dep->sufficient() ? STRONG : WEAK;
                    vote = dep->sufficient() ? "strong" : "weak";
                    vote += " satisfaction";
                    break;

                case STATE_DEACTIVATING:
                    unsatisfied |= PENDING;
                    vote = "pending dissatisfaction";
                    break;

                case STATE_INACTIVE:
                    unsatisfied |= (dep->sufficient() ? WEAK : STRONG);
                    vote = dep->sufficient() ? "weak" : "strong";
                    vote += " dissatisfaction";
                    break;

                case STATE_FAILING:
                    failed = PENDING;
                    vote = " pending failure";
                    break;

                case STATE_FAILED:
                    failed = STRONG;
                    vote = " failure";
                    break;

                default:
                    undetermined |= (dep->sufficient() ? WEAK : STRONG);
                    vote = dep->sufficient() ? "weak" : "strong";
                    vote += " unsettled";
                    break;
                }
            }
            else
            {
                vote = "no vote";
            }

            msg->format("    %s: %s\n", name, vote);
        }

        msg->format(
            "     tally: satisfied=%#02X, unsatisified=%#02X, failed=%#02X, undetermined=%#02X\n",
            satisfied,
            unsatisfied,
            failed,
            undetermined);

        State state = failed & STRONG       ? STATE_FAILED
                    : satisfied & STRONG    ? STATE_ACTIVE
                    : unsatisfied & STRONG  ? STATE_INACTIVE
                    : undetermined & STRONG ? STATE_UNSET
                    : failed & PENDING      ? STATE_FAILING
                    : satisfied & PENDING   ? STATE_ACTIVATING
                    : unsatisfied & PENDING ? STATE_DEACTIVATING
                    : satisfied & WEAK      ? STATE_ACTIVE
                    : unsatisfied & WEAK    ? STATE_INACTIVE
                    : this->settled()       ? this->target_state(this->active())
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
        InvocationStyle invoke_interceptors,
        CascadeStyle cascade_descendants,
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
            cascade_descendants,
            reevaluate,
            on_cancel,
            on_error);
    }

    bool Switch::set_error(
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        InvocationStyle invoke_interceptors,
        CascadeStyle cascade_descendants,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        return this->set_target(
            (error && *error) ? STATE_FAILED : this->target_state(this->active()),
            error,
            attributes,
            clear_existing,
            invoke_interceptors,
            cascade_descendants,
            reevaluate,
            on_cancel,
            on_error);
    }

    bool Switch::set_auto(
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        InvocationStyle invoke_interceptors,
        CascadeStyle cascade_descendants,
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
            cascade_descendants,
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
                    this->name(),
                    this->error());
            }

            for (const SwitchRef &pred : this->get_predecessors())
            {
                errors.merge(pred->errors());
            }
        }
        return errors;
    }

    /// Return this switch's attributes from the local cache.
    const core::types::KeyValueMap &Switch::attributes() const noexcept
    {
        return this->status()->attributes;
    }

    core::types::Value Switch::attribute(const std::string &key) const
    {
        return this->attributes().get(key);
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
        State expected_state = this->target_state(expected);

        if (this->settled_state() != expected_state)
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
                                else
                                {
                                    culprits.insert_or_assign(
                                        name,
                                        std::make_shared<Status>());
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
                culprits.insert_or_assign(this->name(), this->status());
            }
        }

        return culprits;
    }

    DependencyStatusMap Switch::dependency_statuses() const noexcept
    {
        DependencyStatusMap map;

        for (const auto &[pred_name, dep] : this->dependencies())
        {
            auto dep_status = std::make_shared<DependencyStatus>();
            map.insert_or_assign(dep->predecessor_name(), dep_status);

            dep_status->dependency = dep;

            if (const auto &pred = dep->predecessor())
            {
                dep_status->status = pred->status();

                if (const std::optional<State> expected_state = dep->expected_state())
                {
                    dep_status->satisfied = (expected_state == State::STATE_ACTIVE);
                }

                dep_status->dependency_statuses = pred->dependency_statuses();
            }
            else
            {
                dep_status->satisfied = false;
            }
        }

        return map;
    }

    bool Switch::is_in_selection(const SwitchSelection &selection) const noexcept
    {
        if (selection.matches(this->name()))
        {
            return true;
        }
        for (const SwitchName &alias : this->aliases())
        {
            if (selection.matches(alias))
            {
                return true;
            }
        }
        return false;
    }

}  // namespace switchboard
