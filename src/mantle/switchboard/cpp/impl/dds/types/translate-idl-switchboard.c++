/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-switchboard.c++
/// @brief Encode/decode routines for switchboard IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-switchboard.h++"
#include "translate-idl-inline.h++"

namespace idl
{
    // CC::Switchboard::ExceptionHandling
    void encode(switchboard::ExceptionHandling native,
                CC::Switchboard::ExceptionHandling *idl)
    {
        *idl = static_cast<CC::Switchboard::ExceptionHandling>(native);
    }

    void decode(CC::Switchboard::ExceptionHandling idl,
                switchboard::ExceptionHandling *native)
    {
        *native = static_cast<switchboard::ExceptionHandling>(idl);
    }


    // CC::Switchboard::InterceptorPhase
    void encode(switchboard::InterceptorPhase native,
                CC::Switchboard::InterceptorPhase *idl)
    {
        *idl = static_cast<CC::Switchboard::InterceptorPhase>(native);
    }

    void decode(CC::Switchboard::InterceptorPhase idl,
                switchboard::InterceptorPhase *native)
    {
        *native = static_cast<switchboard::InterceptorPhase>(idl);
    }


    // CC::Switchboard::State
    void encode(switchboard::State native,
                CC::Switchboard::State *idl)
    {
        *idl = static_cast<CC::Switchboard::State>(native);
    }

    void decode(CC::Switchboard::State idl,
                switchboard::State *native)
    {
        *native = static_cast<switchboard::State>(idl);
    }

    // CC::Switchboard::Localization
    void encode(const switchboard::LanguageCode &language_code,
                const switchboard::Localization &native,
                CC::Switchboard::Localization *idl)
    {
        idl->language_code(language_code);
        idl->text(native.text);

        idl->target_texts().reserve(native.target_texts.size());
        for (const auto &[target, text] : native.target_texts)
        {
            CC::Switchboard::TargetText tt;
            tt.active(target);
            tt.text(text);
            idl->target_texts().push_back(std::move(tt));
        }

        idl->state_texts().reserve(native.state_texts.size());
        for (const auto &[state, text] : native.state_texts)
        {
            CC::Switchboard::StateText st;
            encode(state, &st.switch_state());
            st.text(text);
            idl->state_texts().push_back(std::move(st));
        }
    }

    void decode(const CC::Switchboard::Localization &idl,
                switchboard::LanguageCode *language_code,
                switchboard::Localization *localization)
    {
        if (language_code)
        {
            *language_code = idl.language_code();
        }
        if (localization)
        {
            localization->text = idl.text();
            for (const CC::Switchboard::TargetText &item : idl.target_texts())
            {
                localization->target_texts[item.active()] = item.text();
                ;
            }
            for (const CC::Switchboard::StateText &item : idl.state_texts())
            {
                auto state = decoded<switchboard::State>(item.switch_state());
                localization->state_texts[state] = item.text();
            }
        }
    }

    // CC::Switchboard::LocalizationList
    void encode(const switchboard::LocalizationMap &native,
                CC::Switchboard::LocalizationList *idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto &[language, localization] : native)
        {
            encode(language, localization, &*it++);
        }
    }

    void decode(const CC::Switchboard::LocalizationList &idl,
                switchboard::LocalizationMap *native)
    {
        for (const auto &desc : idl.list())
        {
            decode(desc, nullptr, &(*native)[desc.language_code()]);
        }
    }

    // CC::Switchboard::DependencyPolarity
    void encode(const switchboard::DependencyPolarity &native,
                CC::Switchboard::DependencyPolarity *idl)
    {
        *idl = static_cast<CC::Switchboard::DependencyPolarity>(native);
    }

    void decode(const CC::Switchboard::DependencyPolarity &idl,
                switchboard::DependencyPolarity *native)
    {
        *native = static_cast<switchboard::DependencyPolarity>(idl);
    }

    // CC::Switchboard::Dependency
    void encode(const switchboard::DependencyRef &native,
                CC::Switchboard::Dependency *idl)
    {
        idl->predecessor(native->predecessor_name());
        idl->trigger_states(native->trigger_states());
        encode(native->polarity(), &idl->polarity());
        idl->hard(native->hard());
        idl->sufficient(native->sufficient());
    }

    void decode(const CC::Switchboard::Dependency &idl,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyRef *native)
    {
        *native = switchboard::Dependency::create_shared(
            provider,
            idl.predecessor(),
            idl.trigger_states(),
            decoded<switchboard::DependencyPolarity>(idl.polarity()),
            idl.hard(),
            idl.sufficient());
    }

    // CC::Switchboard::DependencyList
    void encode(const switchboard::DependencyMap &native,
                CC::Switchboard::DependencyList *idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto &[pred_name, dep] : native)
        {
            encode(dep, &*it++);
        }
    }

    void decode(const CC::Switchboard::DependencyList &idl,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyMap *native)
    {
        for (const CC::Switchboard::Dependency &dep : idl.list())
        {
            decode(dep, provider, &(*native)[dep.predecessor()]);
        }
    }

    // CC::Switchboard::Interceptor
    void encode(const switchboard::InterceptorRef &native,
                CC::Switchboard::Interceptor *idl)
    {
        idl->name(native->name());
        idl->owner(native->owner());
        idl->state_transitions(native->state_transitions());
        encode(native->phase(), &idl->phase());
        idl->asynchronous(native->asynchronous());
        idl->rerun(native->rerun());
        encode(native->on_cancel(), &idl->on_cancel());
        encode(native->on_error(), &idl->on_error());
    }

    void decode(const CC::Switchboard::Interceptor &idl,
                const switchboard::Invocation &invocation,
                switchboard::InterceptorRef *native)
    {
        *native = switchboard::Interceptor::create_shared(
            idl.name(),
            idl.owner(),
            invocation,
            idl.state_transitions(),
            decoded<switchboard::InterceptorPhase>(idl.phase()),
            idl.asynchronous(),
            idl.rerun(),
            decoded<switchboard::ExceptionHandling>(idl.on_cancel()),
            decoded<switchboard::ExceptionHandling>(idl.on_error()));
    }

    // CC::Switchboard::InterceptorList
    void encode(const switchboard::InterceptorMap &native,
                CC::Switchboard::InterceptorList *idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto &[name, icept] : native)
        {
            encode(icept, &*it++);
        }
    }

    void decode(const CC::Switchboard::InterceptorList &idl,
                switchboard::InterceptorMap *native)
    {
        for (const CC::Switchboard::Interceptor &icept : idl.list())
        {
            decode(icept, {}, &(*native)[icept.name()]);
        }
    }

    // // CC::Switchboard::InterceptorID
    // void encode(const switchboard::SwitchName &switch_name,
    //             const switchboard::InterceptorName &interceptor_name,
    //             CC::Switchboard::InterceptorID *idl)
    // {
    //     idl->switch_name(switch_name);
    //     idl->interceptor_name(interceptor_name);
    // }

    // void decode(const CC::Switchboard::InterceptorID &idl,
    //             switchboard::SwitchName *switch_name,
    //             switchboard::InterceptorName *interceptor_name)
    // {
    //     if (switch_name)
    //     {
    //         *switch_name = idl.switch_name();
    //     }
    //     if (interceptor_name)
    //     {
    //         *interceptor_name = idl.interceptor_name();
    //     }
    // }

    // CC::Switchboard::Specification
    void encode(const switchboard::SwitchName &name,
                const switchboard::Specification &native,
                CC::Switchboard::Specification *idl)
    {
        idl->switch_name(name);
        idl->is_primary(native.primary);
        encode(native.localizations, &idl->localizations());
        encode(native.dependencies, &idl->dependencies());
        encode(native.interceptors, &idl->interceptors());
    }

    void decode(const CC::Switchboard::Specification &idl,
                const switchboard::ProviderRef &provider,
                switchboard::SwitchName *name,
                switchboard::Specification *native)
    {
        if (name)
        {
            *name = idl.switch_name();
        }

        if (idl.is_primary().has_value())
        {
            native->primary = idl.is_primary().value();
        }
        decode(idl.localizations(), &native->localizations);
        decode(idl.dependencies(), provider, &native->dependencies);
        decode(idl.interceptors(), &native->interceptors);
    }

    // CC::Switchboard::Status
    void encode(const switchboard::SwitchName &name,
                const switchboard::Status &status,
                CC::Switchboard::Status *idl)
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

    void decode(const CC::Switchboard::Status &idl,
                switchboard::SwitchName *name,
                switchboard::Status *status)
    {
        if (name)
        {
            *name = idl.switch_name();
        }

        decode(idl.current_state(), &status->current_state);
        decode(idl.settled_state(), &status->settled_state);
        status->active = idl.active();
        status->pending = idl.pending();
        if (const CC::Status::Error *errordata = idl.error().get_ptr())
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
    void encode(const switchboard::Switch &sw,
                CC::Switchboard::Switch *idl)
    {
        encode(sw.name(), *sw.spec(), &idl->spec());
        encode(sw.name(), *sw.status(), &idl->status());
    }


    // CC::Switchboard::SwitchList
    void encode(const switchboard::SwitchMap &native,
                CC::Switchboard::SwitchList *idl)
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto &[name, sw] : native)
        {
            CC::Switchboard::Switch &encoded = *it++;
            encode(*sw, &encoded);
        }
    }
}  // namespace idl
