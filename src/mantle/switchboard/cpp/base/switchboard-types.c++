// -*- c++ -*-
//==============================================================================
/// @file switchboard-types.c++
/// @brief Switchboard data types
/// @author Tor Slettnes
//==============================================================================

#include "switch.h++"
#include "status/exceptions.h++"
#include "string/convert.h++"

namespace switchboard
{
    //==========================================================================
    // Switch states

    const core::types::SymbolMap<State> state_names = {
        {STATE_UNSET, "UNSET"},
        {STATE_ACTIVATING, "ACTIVATING"},
        {STATE_ACTIVE, "ACTIVE"},
        {STATE_DEACTIVATING, "DEACTIVATING"},
        {STATE_INACTIVE, "INACTIVE"},
        {STATE_FAILING, "FAILING"},
        {STATE_FAILED, "FAILED"},
    };

    std::ostream &operator<<(std::ostream &stream, State state)
    {
        return state_names.to_stream(stream, state);
    };

    std::istream &operator>>(std::istream &stream, State &state)
    {
        return state_names.from_stream(stream, &state, STATE_UNSET);
    }

    StateSet::StateSet(StateMask mask)
    {
        for (const auto &[state, _] : state_names)
        {
            if ((state & mask) != 0x00)
            {
                this->insert(state);
            }
        }
    }

    std::uint32_t StateSet::to_mask() const
    {
        std::uint32_t mask = 0;
        for (State state : *this)
        {
            mask |= static_cast<std::uint32_t>(state);
        }
        return mask;
    }

    void StateSet::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        for (const auto &[state, name] : state_names)
        {
            if (this->count(state))
            {
                tvlist->append({}, name);
            }
        }
    }

    //==========================================================================
    // Exception Handling

    const core::types::SymbolMap<ExceptionHandling> exceptionhandling_names = {
        {EH_DEFAULT, "DEFAULT"},
        {EH_IGNORE, "IGNORE"},
        {EH_ABORT, "ABORT"},
        {EH_FAIL, "FAIL"},
        {EH_REVERT, "REVERT"},
    };

    std::ostream &operator<<(std::ostream &stream, ExceptionHandling eh)
    {
        return exceptionhandling_names.to_stream(stream, eh);
    };

    std::istream &operator>>(std::istream &stream, ExceptionHandling &eh)
    {
        return exceptionhandling_names.from_stream(stream, &eh, EH_DEFAULT);
    }

    //==========================================================================
    // CascadeStyle: How to propagate state change to descendants.

    const core::types::SymbolMap<CascadeStyle> cascadestyle_names = {
        {CascadeStyle::NONE, "NONE"},
        {CascadeStyle::ASYNC, "ASYNC"},
        {CascadeStyle::WAIT_DIRECT, "WAIT_DIRECT"},
        {CascadeStyle::WAIT_RECURSIVE, "WAIT_RECURSIVE"},
    };

    std::ostream &operator<<(std::ostream &stream, CascadeStyle style)
    {
        return cascadestyle_names.to_stream(stream, style);
    }

    std::istream &operator>>(std::istream &stream, CascadeStyle &style)
    {
        return cascadestyle_names.from_stream(
            stream,
            &style,
            CascadeStyle::DEFAULT);
    }

    //==========================================================================
    // DependencyPolarity

    const core::types::SymbolMap<DependencyPolarity> depdir_names = {
        {DependencyPolarity::POSITIVE, "POSITIVE"},
        {DependencyPolarity::NEGATIVE, "NEGATIVE"},
        {DependencyPolarity::TOGGLE, "TOGGLE"},
    };

    std::ostream &operator<<(std::ostream &stream, DependencyPolarity dir)
    {
        return depdir_names.to_stream(stream, dir);
    }

    std::istream &operator>>(std::istream &stream, DependencyPolarity &dir)
    {
        return depdir_names.from_stream(stream, &dir);
    }

    //==========================================================================
    // SwitchSelection

    SwitchSelection::SwitchSelection()
        : is_regex(false)
    {
    }

    SwitchSelection::SwitchSelection(const std::string &switch_name)
        : patterns({switch_name}),
          is_regex(false)
    {
    }

    SwitchSelection::SwitchSelection(const std::vector<std::string> &patterns,
                                     bool is_regex)
        : patterns(patterns),
          is_regex(is_regex)
    {
        if (is_regex)
        {
            this->regex_patterns.reserve(patterns.size());
            for (const std::string &pattern : patterns)
            {
                this->regex_patterns.emplace_back(pattern);
            }
        }
    }

    bool SwitchSelection::matches(const SwitchName &switch_name) const
    {
        if (this->is_regex)
        {
            for (const std::regex &rx : this->regex_patterns)
            {
                if (std::regex_match(switch_name, rx))
                {
                    return true;
                }
            }
        }
        else
        {
            for (const std::string &pattern : this->patterns)
            {
                if (core::platform::path->filename_match(pattern, switch_name, true))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void SwitchSelection::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("patterns",
                       core::types::ValueList::create_shared_from(this->patterns));
        tvlist->append("ix_regex", this->is_regex);
    }

    //==========================================================================
    // Localization

    void Localization::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if_value("description", this->description);
        tvlist->append_if_value("activate_text", this->activate_text);
        tvlist->append_if_value("deactivate_text", this->deactivate_text);

        if (!this->state_texts.empty())
        {
            core::types::TaggedValueList texts;
            for (const auto &[state, text] : this->state_texts)
            {
                texts.append_if_value(state_names.try_to_string(state), text);
            }
            tvlist->append("state_texts", texts);
        }
    }

    bool operator==(const Localization &lhs, const Localization &rhs)
    {
        return ((lhs.description == rhs.description) &&
                (lhs.activate_text == rhs.activate_text) &&
                (lhs.deactivate_text == rhs.deactivate_text) &&
                (lhs.state_texts == rhs.state_texts));
    }

    bool operator!=(const Localization &lhs, const Localization &rhs)
    {
        return !(lhs == rhs);
    }

    //==========================================================================
    // Specification

    bool operator==(const Specification &lhs, const Specification &rhs)
    {
        return ((lhs.primary == rhs.primary) &&
                (lhs.aliases == rhs.aliases) &&
                (lhs.dependencies == rhs.dependencies) &&
                (lhs.interceptors == rhs.interceptors) &&
                (lhs.localizations == rhs.localizations));
    }

    bool operator!=(const Specification &lhs, const Specification &rhs)
    {
        return !(lhs == rhs);
    }

    void Specification::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        if (!this->aliases.empty())
        {
            tvlist->append("aliases", core::types::ValueList::create_from(this->aliases));
        }

        if (this->primary)
        {
            tvlist->append("primary", this->primary);
        }

        if (!this->localizations.empty())
        {
            core::types::TaggedValueList localizations;
            for (const auto &[language, localization] : this->localizations)
            {
                localizations.append_if_value(language, localization.as_kvmap());
            }
            tvlist->append("localizations", localizations);
        }

        if (!this->interceptors.empty())
        {
            core::types::ValueList interceptors;
            for (const auto &[key, interceptor] : this->interceptors)
            {
                interceptors.push_back(interceptor->as_tvlist());
            }
            tvlist->append("interceptors", interceptors);
        }

        if (!this->dependencies.empty())
        {
            core::types::KeyValueMap dependencies;
            for (const auto &[predecessor_name, spec] : this->dependencies)
            {
                dependencies.insert_or_assign(predecessor_name, spec->as_kvmap());
            }
            tvlist->append("dependencies", dependencies);
        }
    }

    //==========================================================================
    /// @class Status

    void Status::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("current_state", state_names.try_to_string(this->current_state));
        tvlist->append("settled_state", state_names.try_to_string(this->settled_state));
        tvlist->append("active", this->active);
        tvlist->append("pending", this->pending);
        if (this->error)
        {
            tvlist->append("error", this->error->as_tvlist());
        }
        if (!this->attributes.empty())
        {
            tvlist->append("attributes", this->attributes);
        }
    }

    bool operator==(const Status &lhs, const Status &rhs)
    {
        return ((lhs.current_state == rhs.current_state) &&
                (lhs.settled_state == rhs.settled_state) &&
                (lhs.active == rhs.active) &&
                (lhs.error == rhs.error) &&
                (lhs.attributes == rhs.attributes));
    }

    bool operator!=(const Status &lhs, const Status &rhs)
    {
        return !(lhs == rhs);
    }

}  // namespace switchboard
