/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-switchboard.c++
/// @brief Encode/decode routines for switchboard IDL types
/// @author Tor Slettnes
//==============================================================================

#include "translate-idl-switchboard.h++"
#include "translate-idl-inline.h++"

namespace cc::idl
{
    // CC::Switchboard::ExceptionHandling
    void encode(cc::platform::switchboard::ExceptionHandling native,
                CC::Switchboard::ExceptionHandling* idl)
    {
        *idl = static_cast<CC::Switchboard::ExceptionHandling>(native);
    }

    void decode(CC::Switchboard::ExceptionHandling idl,
                cc::platform::switchboard::ExceptionHandling* native)
    {
        *native = static_cast<cc::platform::switchboard::ExceptionHandling>(idl);
    }

    // CC::Switchboard::InterceptorPhase
    void encode(cc::platform::switchboard::InterceptorPhase native,
                CC::Switchboard::InterceptorPhase* idl)
    {
        *idl = static_cast<CC::Switchboard::InterceptorPhase>(native);
    }

    void decode(CC::Switchboard::InterceptorPhase idl,
                cc::platform::switchboard::InterceptorPhase* native)
    {
        *native = static_cast<cc::platform::switchboard::InterceptorPhase>(idl);
    }

    // CC::Switchboard::InvocationStyle
    void encode(cc::platform::switchboard::InvocationStyle native,
                CC::Switchboard::InvocationStyle* idl)
    {
        *idl = static_cast<CC::Switchboard::InvocationStyle>(native);
    }

    void decode(CC::Switchboard::InvocationStyle idl,
                cc::platform::switchboard::InvocationStyle* native)
    {
        *native = static_cast<cc::platform::switchboard::InvocationStyle>(idl);
    }

    // CC::Switchboard::CascadeStyle
    void encode(cc::platform::switchboard::CascadeStyle native,
                CC::Switchboard::CascadeStyle* idl)
    {
        *idl = static_cast<CC::Switchboard::CascadeStyle>(native);
    }

    void decode(CC::Switchboard::CascadeStyle idl,
                cc::platform::switchboard::CascadeStyle* native)
    {
        *native = static_cast<cc::platform::switchboard::CascadeStyle>(idl);
    }

    // CC::Switchboard::State
    void encode(cc::platform::switchboard::State native,
                CC::Switchboard::State* idl)
    {
        *idl = static_cast<CC::Switchboard::State>(native);
    }

    void decode(CC::Switchboard::State idl,
                cc::platform::switchboard::State* native)
    {
        *native = static_cast<cc::platform::switchboard::State>(idl);
    }

    // CC::Switchboard::AliasList
    void encode(cc::platform::switchboard::SwitchAliases native,
                CC::Switchboard::NameList* idl)
    {
        idl->insert(idl->end(), native.begin(), native.end());
    }

    void decode(CC::Switchboard::NameList idl,
                cc::platform::switchboard::SwitchAliases* native)
    {
        native->insert(idl.begin(), idl.end());
    }

    // CC::Switchboard::SwitchSelection
    void encode(const cc::platform::switchboard::SwitchSelection& native,
                CC::Switchboard::SwitchSelection* idl)
    {
        idl->patterns(native.patterns);
        idl->is_regex(native.is_regex);
        idl->with_ancestors(native.with_ancestors);
    }

    void decode(const CC::Switchboard::SwitchSelection& idl,
                cc::platform::switchboard::SwitchSelection* native)
    {
        native->patterns = idl.patterns();
        native->is_regex = idl.is_regex();
        native->with_ancestors = idl.with_ancestors();
    }

    // CC::Switchboard::Localization
    void encode(const cc::platform::switchboard::LanguageCode& language_code,
                const cc::platform::switchboard::Localization& native,
                CC::Switchboard::Localization* idl)
    {
        idl->language_code(language_code);
        idl->description(native.description);
        idl->activate_text(native.activate_text);
        idl->deactivate_text(native.deactivate_text);

        idl->state_texts().reserve(native.state_texts.size());
        for (const auto& [state, text] : native.state_texts)
        {
            CC::Switchboard::StateText st;
            encode(state, &st.switch_state());
            st.text(text);
            idl->state_texts().push_back(std::move(st));
        }
    }

    void decode(const CC::Switchboard::Localization& idl,
                cc::platform::switchboard::LanguageCode* language_code,
                cc::platform::switchboard::Localization* localization)
    {
        if (language_code)
        {
            *language_code = idl.language_code();
        }
        if (localization)
        {
            localization->description = idl.description();
            localization->activate_text = idl.activate_text();
            localization->deactivate_text = idl.deactivate_text();
            for (const CC::Switchboard::StateText& item : idl.state_texts())
            {
                auto state = decoded<cc::platform::switchboard::State>(item.switch_state());
                localization->state_texts[state] = item.text();
            }
        }
    }

    // CC::Switchboard::LocalizationList
    void encode(const cc::platform::switchboard::LocalizationMap& native,
                CC::Switchboard::LocalizationList* idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto& [language, localization] : native)
        {
            encode(language, localization, &*it++);
        }
    }

    void decode(const CC::Switchboard::LocalizationList& idl,
                cc::platform::switchboard::LocalizationMap* native)
    {
        for (const auto& desc : idl.list())
        {
            decode(desc, nullptr, &(*native)[desc.language_code()]);
        }
    }

    // CC::Switchboard::DependencyPolarity
    void encode(const cc::platform::switchboard::DependencyPolarity& native,
                CC::Switchboard::DependencyPolarity* idl)
    {
        *idl = static_cast<CC::Switchboard::DependencyPolarity>(native);
    }

    void decode(const CC::Switchboard::DependencyPolarity& idl,
                cc::platform::switchboard::DependencyPolarity* native)
    {
        *native = static_cast<cc::platform::switchboard::DependencyPolarity>(idl);
    }

    // CC::Switchboard::Dependency
    void encode(const cc::platform::switchboard::DependencyRef& native,
                CC::Switchboard::Dependency* idl)
    {
        idl->predecessor(native->predecessor_name());
        idl->trigger_states(native->trigger_states().to_mask());
        encode(native->polarity(), &idl->polarity());
        idl->hard(native->hard());
        idl->sufficient(native->sufficient());
    }

    void decode(const CC::Switchboard::Dependency& idl,
                const cc::platform::switchboard::ProviderRef& provider,
                cc::platform::switchboard::DependencyRef* native)
    {
        *native = cc::platform::switchboard::Dependency::create_shared(
            provider,
            idl.predecessor(),
            idl.trigger_states(),
            decoded<cc::platform::switchboard::DependencyPolarity>(idl.polarity()),
            idl.hard(),
            idl.sufficient());
    }

    // CC::Switchboard::DependencyList
    void encode(const cc::platform::switchboard::DependencyMap& native,
                CC::Switchboard::DependencyList* idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto& [pred_name, dep] : native)
        {
            encode(dep, &*it++);
        }
    }

    void decode(const CC::Switchboard::DependencyList& idl,
                const cc::platform::switchboard::ProviderRef& provider,
                cc::platform::switchboard::DependencyMap* native)
    {
        for (const CC::Switchboard::Dependency& dep : idl.list())
        {
            decode(dep, provider, &(*native)[dep.predecessor()]);
        }
    }

    // CC::Switchboard::Interceptor
    void encode(const cc::platform::switchboard::InterceptorRef& native,
                CC::Switchboard::Interceptor* idl)
    {
        idl->name(native->name());
        idl->owner(native->owner());
        idl->state_transitions(native->state_transitions().to_mask());
        encode(native->phase(), &idl->phase());
        idl->asynchronous(native->asynchronous());
        idl->rerun(native->rerun());
        encode(native->on_cancel(), &idl->on_cancel());
        encode(native->on_error(), &idl->on_error());
    }

    void decode(const CC::Switchboard::Interceptor& idl,
                const std::optional<cc::platform::switchboard::InterceptorOwner>& owner,
                const cc::platform::switchboard::Invocation& invocation,
                cc::platform::switchboard::InterceptorRef* native)
    {
        *native = cc::platform::switchboard::Interceptor::create_shared(
            idl.name(),
            owner.value_or(idl.owner()),
            invocation,
            idl.state_transitions(),
            decoded<cc::platform::switchboard::InterceptorPhase>(idl.phase()),
            idl.asynchronous(),
            idl.rerun(),
            decoded<cc::platform::switchboard::ExceptionHandling>(idl.on_cancel()),
            decoded<cc::platform::switchboard::ExceptionHandling>(idl.on_error()));
    }

    // CC::Switchboard::InterceptorList
    void encode(const cc::platform::switchboard::InterceptorMap& native,
                CC::Switchboard::InterceptorList* idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto& [key, icept] : native)
        {
            encode(icept, &*it++);
        }
    }

    void decode(const CC::Switchboard::InterceptorList& idl,
                cc::platform::switchboard::InterceptorMap* native)
    {
        for (const CC::Switchboard::Interceptor& icept : idl.list())
        {
            decode(
                icept,  // interceptor
                {},     // owner
                {},     // invocation
                &(*native)[icept.name()]);
        }
    }

    // CC::Switchboard::Specification
    void encode(const cc::platform::switchboard::SwitchName& name,
                const cc::platform::switchboard::Specification& native,
                CC::Switchboard::Specification* idl)
    {
        idl->switch_name(name);
        idl->is_primary(native.primary);
        encode(native.aliases, &idl->aliases());
        encode(native.localizations, &idl->localizations());
        encode(native.dependencies, &idl->dependencies());
        encode(native.interceptors, &idl->interceptors());
    }

    void decode(const CC::Switchboard::Specification& idl,
                const cc::platform::switchboard::ProviderRef& provider,
                cc::platform::switchboard::SwitchName* name,
                cc::platform::switchboard::Specification* native)
    {
        if (name)
        {
            *name = idl.switch_name();
        }

        if (idl.is_primary().has_value())
        {
            native->primary = idl.is_primary().value();
        }

        decode(idl.aliases(), &native->aliases);
        decode(idl.localizations(), &native->localizations);
        decode(idl.dependencies(), provider, &native->dependencies);
        decode(idl.interceptors(), &native->interceptors);
    }

    // CC::Switchboard::Status
    void encode(const cc::platform::switchboard::SwitchName& name,
                const cc::platform::switchboard::Status& status,
                CC::Switchboard::Status* idl)
    {
        idl->switch_name(name);
        encode(status.current_state, &idl->current_state());
        encode(status.settled_state, &idl->settled_state());
        idl->active(status.active);
        idl->pending(status.pending);
        if (status.error)
        {
            idl->error(encoded<CC::Status::Error>(*status.error));
        }
        else
        {
            idl->error().reset();
        }
        encode(status.attributes, &idl->attributes());
    }

    void decode(const CC::Switchboard::Status& idl,
                cc::platform::switchboard::SwitchName* name,
                cc::platform::switchboard::Status* status)
    {
        if (name)
        {
            *name = idl.switch_name();
        }

        decode(idl.current_state(), &status->current_state);
        decode(idl.settled_state(), &status->settled_state);
        status->active = idl.active();
        status->pending = idl.pending();
        if (const CC::Status::Error* errordata = idl.error().get_ptr())
        {
            decode_shared(*errordata, &status->error);
        }
        else
        {
            status->error.reset();
        }
        decode(idl.attributes(), &status->attributes);
    }

    // CC::Switchboard::Switch
    void encode(const cc::platform::switchboard::Switch& sw,
                CC::Switchboard::Switch* idl)
    {
        encode(sw.name(), *sw.spec(), &idl->spec());
        encode(sw.name(), *sw.status(), &idl->status());
    }

    // CC::Switchboard::SwitchList
    void encode(const cc::platform::switchboard::SwitchMap& native,
                CC::Switchboard::SwitchList* idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto& [name, sw] : native)
        {
            CC::Switchboard::Switch& encoded = *it++;
            encode(*sw, &encoded);
        }
    }
}  // namespace cc::idl
