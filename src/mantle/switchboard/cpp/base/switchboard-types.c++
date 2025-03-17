// -*- c++ -*-
//==============================================================================
/// @file switchboard-types.c++
/// @brief Switchboard data types
/// @author Tor Slettnes <tor@slett.net>
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
    /// @struct Localization

    void Localization::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if_value("description", this->description);

        if (!this->target_texts.empty())
        {
            core::types::TaggedValueList texts;
            for (const auto &[target, text] : this->target_texts)
            {
                texts.append_if_value(target ? "ON" : "OFF", text);
            }
            tvlist->append("target_texts", texts);
        }

        if (!this->state_texts.empty())
        {
            core::types::TaggedValueList texts;
            for (const auto &[state, text] : this->state_texts)
            {
                texts.append_if_value(state_names.to_string(state), text);
            }
            tvlist->append("state_texts", texts);
        }
    }

    bool operator==(const Localization &lhs, const Localization &rhs)
    {
        return ((lhs.description == rhs.description) &&
                (lhs.target_texts == rhs.target_texts) &&
                (lhs.state_texts == rhs.state_texts));
    }

    bool operator!=(const Localization &lhs, const Localization &rhs)
    {
        return !(lhs == rhs);
    }

    //==========================================================================
    /// @class Specification

    bool operator==(const Specification &lhs, const Specification &rhs)
    {
        return ((lhs.primary == rhs.primary) &&
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
        tvlist->append("primary", this->primary);

        if (!this->localizations.empty())
        {
            core::types::TaggedValueList localizations;
            for (const auto &[language, localization] : this->localizations)
            {
                localizations.append_if_value(language, localization.as_tvlist());
            }
            tvlist->append("localizations", localizations);
        }

        if (!this->interceptors.empty())
        {
            core::types::TaggedValueList interceptors;
            for (const auto &[name, spec] : this->interceptors)
            {
                interceptors.append_if_value(name, spec->to_string());
            }
            tvlist->append("interceptors", interceptors);
        }

        if (!this->dependencies.empty())
        {
            core::types::TaggedValueList dependencies;
            for (const auto &[predecessor, spec] : this->dependencies)
            {
                dependencies.append_if_value(predecessor, spec->to_string());
            }
            tvlist->append("dependencies", dependencies);
        }
    }

    //==========================================================================
    /// @class Status

    void Status::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("current_state", state_names.to_string(this->current_state));
        tvlist->append("settled_state", state_names.to_string(this->settled_state));
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
