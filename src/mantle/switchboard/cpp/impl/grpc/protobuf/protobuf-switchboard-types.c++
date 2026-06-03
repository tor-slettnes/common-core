// -*- c++ -*-
//==============================================================================
/// @file protobuf-switchboard-types.c++
/// @brief conversions to/from Protocol Buffer messages in "switchboard.proto"
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-switchboard-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

namespace cc::protobuf
{
    //==========================================================================
    // State

    void encode(platform::switchboard::State native,
                platform::switchboard::protobuf::State *proto)

    {
        *proto = static_cast<platform::switchboard::protobuf::State>(native);
    }

    void decode(platform::switchboard::protobuf::State proto,
                platform::switchboard::State *native)
    {
        *native = static_cast<platform::switchboard::State>(proto);
    }

    //==========================================================================
    // State set

    void encode(const std::set<platform::switchboard::State> &states,
                google::protobuf::RepeatedField<int> *items)
    {
        items->Clear();
        items->Reserve(states.size());
        for (platform::switchboard::State state : states)
        {
            items->Add(state);
        }
    }

    void decode(const google::protobuf::RepeatedField<int> &items,
                std::set<platform::switchboard::State> *states)
    {
        states->clear();
        for (int state : items)
        {
            for (uint mask = 0x01; mask <= state; mask <<= 1)
            {
                if ((state & mask) != 0x00)
                {
                    states->insert(static_cast<platform::switchboard::State>(mask));
                }
            }
        }
    }

    //==========================================================================
    // SwitchIdentifier

    void encode(const std::string &name, platform::switchboard::protobuf::SwitchIdentifier *msg)
    {
        msg->set_switch_name(name);
    }

    void decode(const platform::switchboard::protobuf::SwitchIdentifier &msg, std::string *name)
    {
        *name = msg.switch_name();
    }

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string> &names,
                platform::switchboard::protobuf::SwitchIdentifiers *msg)
    {
        msg->mutable_switch_names()->Reserve(names.size());
        for (const std::string &name : names)
        {
            msg->add_switch_names(name);
        }
    }

    void decode(const platform::switchboard::protobuf::SwitchIdentifiers &msg,
                std::vector<std::string> *names)
    {
        assign_to_vector(msg.switch_names(), names);
    }

    //==========================================================================
    // SwitchSet

    void encode(const platform::switchboard::SwitchSet &set,
                platform::switchboard::protobuf::SwitchIdentifiers *msg)
    {
        msg->mutable_switch_names()->Reserve(set.size());
        for (const auto &sw : set)
        {
            msg->add_switch_names(sw->name());
        }
    }

    //==========================================================================
    // SwitchSelection

    void encode(const platform::switchboard::SwitchSelection &patterns,
                platform::switchboard::protobuf::SwitchSelection *msg)
    {
        assign_repeated(patterns.patterns, msg->mutable_patterns());
        msg->set_is_regex(patterns.is_regex);
        msg->set_with_ancestors(patterns.with_ancestors);
    }

    void decode(const platform::switchboard::protobuf::SwitchSelection &msg,
                platform::switchboard::SwitchSelection *patterns)
    {
        assign_to_vector(msg.patterns(), &patterns->patterns);
        patterns->is_regex = msg.is_regex();
        patterns->with_ancestors = msg.with_ancestors();
    }

    //==========================================================================
    // Specification

    void encode(const platform::switchboard::Specification &spec,
                platform::switchboard::protobuf::Specification *msg)
    {
        msg->set_is_primary(spec.primary);
        encode(spec.aliases, msg->mutable_aliases());
        encode(spec.localizations, msg->mutable_localizations());
        encode(spec.dependencies, msg->mutable_dependencies());
        encode(spec.interceptors, msg->mutable_interceptors());
    }

    void decode(const platform::switchboard::protobuf::Specification &msg,
                const platform::switchboard::ProviderRef &provider,
                platform::switchboard::Specification *spec)
    {
        if (msg.has_is_primary())
        {
            spec->primary = msg.is_primary();
        }

        decode(msg.aliases(), &spec->aliases);
        decode(msg.localizations(), &spec->localizations);
        decode(msg.dependencies(), provider, &spec->dependencies);
        decode(msg.interceptors(), &spec->interceptors);
    }

    //==========================================================================
    // Status

    void encode(const platform::switchboard::Status &status,
                platform::switchboard::protobuf::Status *msg)
    {
        msg->set_current_state(encoded<platform::switchboard::protobuf::State>(status.current_state));
        msg->set_settled_state(encoded<platform::switchboard::protobuf::State>(status.settled_state));
        msg->set_active(status.active);
        msg->set_pending(status.pending);

        if (status.error)
        {
            encode(*status.error, msg->mutable_error());
        }

        if (status.attributes.size())
        {
            encode(status.attributes, msg->mutable_attributes());
        }
    }

    void decode(const platform::switchboard::protobuf::Status &msg,
                platform::switchboard::Status *status)
    {
        status->current_state = decoded<platform::switchboard::State>(msg.current_state());
        status->settled_state = decoded<platform::switchboard::State>(msg.settled_state());
        status->active = msg.active();
        status->pending = msg.pending();
        if (msg.has_error())
        {
            decode_shared(msg.error(), &status->error);
        }
        else
        {
            status->error.reset();
        }
        decode(msg.attributes(), &status->attributes);
    }

    //==========================================================================
    // StatusMap

    void encode(const platform::switchboard::StatusMap &statusmap,
                platform::switchboard::protobuf::StatusMap *msg)
    {
        msg->clear_map();
        auto *map = msg->mutable_map();
        for (const auto &[id, status] : statusmap)
        {
            encode_shared(status, &(*map)[id]);
        }
    }

    void decode(const platform::switchboard::protobuf::StatusMap &msg,
                platform::switchboard::StatusMap *statusmap)
    {
        statusmap->clear();
        for (const auto &[id, protostatus] : msg.map())
        {
            decode_shared(protostatus, &(*statusmap)[id]);
        }
    }

    //==========================================================================
    // ErrorMap

    void encode(const platform::switchboard::ErrorMap &errormap,
                platform::switchboard::protobuf::ErrorMap *msg)
    {
        msg->clear_map();
        auto *map = msg->mutable_map();
        for (const auto &[switch_name, error] : errormap)
        {
            encode_shared(error, &(*map)[switch_name]);
        }
    }

    //==========================================================================
    // Aliases

    void encode(const std::set<platform::switchboard::SwitchName> &aliases,
                google::protobuf::RepeatedPtrField<std::string> *items)
    {
        items->Clear();
        items->Reserve(aliases.size());
        for (const platform::switchboard::SwitchName &alias : aliases)
        {
            items->Add()->assign(alias);
        }
    }

    void decode(const google::protobuf::RepeatedPtrField<std::string> &items,
                std::set<platform::switchboard::SwitchName> *aliases)
    {
        aliases->clear();
        aliases->insert(items.begin(), items.end());
    }

    //==========================================================================
    // Localization

    void encode(const platform::switchboard::Localization &localization,
                platform::switchboard::protobuf::Localization *msg)
    {
        msg->set_description(localization.description);
        msg->set_activate_text(localization.activate_text);
        msg->set_deactivate_text(localization.deactivate_text);

        auto &state_map = *msg->mutable_state_texts();
        for (const auto &[state, text] : localization.state_texts)
        {
            state_map[state] = text;
        }
    }

    void decode(const platform::switchboard::protobuf::Localization &msg,
                platform::switchboard::Localization *localization)
    {
        localization->description = msg.description();
        localization->activate_text = msg.activate_text();
        localization->deactivate_text = msg.deactivate_text();

        for (const auto &[state, text] : msg.state_texts())
        {
            localization->state_texts.insert_or_assign(
                static_cast<platform::switchboard::State>(state),
                text);
        }
    }

    //==========================================================================
    // LocalizationMap

    void encode(const platform::switchboard::LocalizationMap &map,
                platform::switchboard::protobuf::LocalizationMap *msg)
    {
        auto &protomap = *msg->mutable_map();
        for (const auto &[language_code, localization] : map)
        {
            encode(localization, &protomap[language_code]);
        }
    }

    void decode(const platform::switchboard::protobuf::LocalizationMap &msg,
                platform::switchboard::LocalizationMap *map)
    {
        map->clear();
        for (const auto &[language, localization] : msg.map())
        {
            decode(localization, &(*map)[language]);
        }
    }

    //==========================================================================
    // DependencyPolarity

    void encode(platform::switchboard::DependencyPolarity native,
                platform::switchboard::protobuf::DependencyPolarity *proto)
    {
        *proto = static_cast<platform::switchboard::protobuf::DependencyPolarity>(native);
    }

    void decode(platform::switchboard::protobuf::DependencyPolarity proto,
                platform::switchboard::DependencyPolarity *native)
    {
        *native = static_cast<platform::switchboard::DependencyPolarity>(proto);
    }

    //==========================================================================
    // Dependency

    void encode(const platform::switchboard::DependencyRef &native,
                platform::switchboard::protobuf::Dependency *proto)
    {
        encode(native->trigger_states(), proto->mutable_trigger_states());
        proto->set_polarity(
            encoded<platform::switchboard::protobuf::DependencyPolarity>(
                native->polarity()));

        proto->set_hard(native->hard());
        proto->set_sufficient(native->sufficient());
    }

    void decode(const platform::switchboard::protobuf::Dependency &proto,
                const platform::switchboard::ProviderRef &provider,
                const platform::switchboard::SwitchName &predecessor_name,
                platform::switchboard::DependencyRef *native)
    {
        *native = platform::switchboard::Dependency::create_shared(
            provider,
            predecessor_name,
            decoded<platform::switchboard::StateSet>(proto.trigger_states()),
            decoded<platform::switchboard::DependencyPolarity>(proto.polarity()),
            proto.hard(),
            proto.sufficient());
    }

    //==========================================================================
    // DependencyMap

    void encode(const platform::switchboard::DependencyMap &map,
                platform::switchboard::protobuf::DependencyMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, dep] : map)
        {
            encode(dep, &encoded_map[name]);
        }
    }

    void decode(const platform::switchboard::protobuf::DependencyMap &msg,
                const platform::switchboard::ProviderRef &provider,
                platform::switchboard::DependencyMap *map)
    {
        for (const auto &[predecessor_name, spec] : msg.map())
        {
            decode(spec, provider, predecessor_name, &(*map)[predecessor_name]);
        }
    }

    //==========================================================================
    // DependencyStatus

    void encode(const platform::switchboard::DependencyStatus &native,
                platform::switchboard::protobuf::DependencyStatus *proto)
    {
        encode(native.dependency, proto->mutable_dependency());

        if (native.status)
        {
            encode_shared(native.status, proto->mutable_status());
        }

        if (!native.dependency_statuses.empty())
        {
            encode(native.dependency_statuses, proto->mutable_dependency_statuses());
        }

        if (native.satisfied.has_value())
        {
            proto->set_satisfied(native.satisfied.value());
        }
    }

    void decode(const platform::switchboard::protobuf::DependencyStatus &proto,
                const platform::switchboard::ProviderRef &provider,
                const platform::switchboard::SwitchName &predecessor_name,
                platform::switchboard::DependencyStatus *native)
    {
        decode(proto.dependency(), provider, predecessor_name, &native->dependency);

        if (proto.has_status())
        {
            decode_shared(proto.status(), &native->status);
        }

        if (proto.has_dependency_statuses())
        {
            decode(proto.dependency_statuses(), provider, &native->dependency_statuses);
        }

        if (proto.has_satisfied())
        {
            native->satisfied = proto.satisfied();
        }
    }

    //==========================================================================
    // DependencyStatusMap

    void encode(const platform::switchboard::DependencyStatusMap &native,
                platform::switchboard::protobuf::DependencyStatusMap *proto)
    {
        auto &protomap = *proto->mutable_map();
        for (const auto &[switch_name, dep_status] : native)
        {
            encode_shared(dep_status, &protomap[switch_name]);
        }
    }

    void decode(const platform::switchboard::protobuf::DependencyStatusMap &proto,
                const platform::switchboard::ProviderRef &provider,
                platform::switchboard::DependencyStatusMap *native)
    {
        native->clear();
        for (const auto &[switch_name, proto_dep_status] : proto.map())
        {
            native->insert_or_assign(
                switch_name,
                decoded_shared<platform::switchboard::DependencyStatus>(
                    proto_dep_status,
                    provider,
                    switch_name));
        }
    }

    //==========================================================================
    // Interceptor

    void encode(const platform::switchboard::InterceptorRef &native,
                platform::switchboard::protobuf::InterceptorSpec *proto)
    {
        proto->set_owner(native->owner());
        encode(native->state_transitions(), proto->mutable_state_transitions());
        proto->set_phase(
            encoded<platform::switchboard::protobuf::InterceptorPhase>(
                native->phase()));
        proto->set_asynchronous(native->asynchronous());
        proto->set_rerun(native->rerun());
        proto->set_on_cancel(
            encoded<platform::switchboard::protobuf::ExceptionHandling>(
                native->on_cancel()));
        proto->set_on_error(
            encoded<platform::switchboard::protobuf::ExceptionHandling>(
                native->on_error()));
    }

    void decode(const platform::switchboard::protobuf::InterceptorSpec &proto,
                const platform::switchboard::InterceptorName &name,
                const platform::switchboard::InterceptorOwner &owner,
                const platform::switchboard::Invocation &invocation,
                platform::switchboard::InterceptorRef *native)
    {
        *native = platform::switchboard::Interceptor::create_shared(
            name,
            owner,
            invocation,
            decoded<platform::switchboard::StateSet>(proto.state_transitions()),
            decoded<platform::switchboard::InterceptorPhase>(proto.phase()),
            proto.asynchronous(),
            proto.rerun(),
            decoded<platform::switchboard::ExceptionHandling>(proto.on_cancel()),
            decoded<platform::switchboard::ExceptionHandling>(proto.on_error()));
    }

    //==========================================================================
    // InterceptorMap

    void encode(const platform::switchboard::InterceptorMap &map,
                platform::switchboard::protobuf::InterceptorMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[key, ic] : map)
        {
            encode(ic, &encoded_map[key]);
        }
    }

    void decode(const platform::switchboard::protobuf::InterceptorMap &proto,
                platform::switchboard::InterceptorMap *native)
    {
        for (const auto &[key, spec] : proto.map())
        {
            decode(spec,              // proto
                   key,               // name
                   spec.owner(),      // owner
                   {},                // invocation
                   &(*native)[key]);  // native
        }
    }

    //==========================================================================
    // InterceptorPhase

    void encode(platform::switchboard::InterceptorPhase native,
                platform::switchboard::protobuf::InterceptorPhase *proto)
    {
        *proto = static_cast<platform::switchboard::protobuf::InterceptorPhase>(native);
    }

    void decode(platform::switchboard::protobuf::InterceptorPhase proto,
                platform::switchboard::InterceptorPhase *native)
    {
        *native = static_cast<platform::switchboard::InterceptorPhase>(proto);
    }

    //==========================================================================
    // ExceptionHandling

    void encode(platform::switchboard::ExceptionHandling native,
                platform::switchboard::protobuf::ExceptionHandling *proto)
    {
        *proto = static_cast<platform::switchboard::protobuf::ExceptionHandling>(native);
    }

    void decode(platform::switchboard::protobuf::ExceptionHandling proto,
                platform::switchboard::ExceptionHandling *native)
    {
        *native = static_cast<platform::switchboard::ExceptionHandling>(proto);
    }

    //==========================================================================
    // InvocationStyle

    void encode(platform::switchboard::InvocationStyle native,
                platform::switchboard::protobuf::InvocationStyle *proto)
    {
        *proto = static_cast<platform::switchboard::protobuf::InvocationStyle>(native);
    }

    void decode(platform::switchboard::protobuf::InvocationStyle proto,
                platform::switchboard::InvocationStyle *native)
    {
        *native = static_cast<platform::switchboard::InvocationStyle>(proto);
    }

    //==========================================================================
    // CascadeStyle

    void encode(platform::switchboard::CascadeStyle native,
                platform::switchboard::protobuf::CascadeStyle *proto)
    {
        *proto = static_cast<platform::switchboard::protobuf::CascadeStyle>(native);
    }

    void decode(platform::switchboard::protobuf::CascadeStyle proto,
                platform::switchboard::CascadeStyle *native)
    {
        *native = static_cast<platform::switchboard::CascadeStyle>(proto);
    }

    //==========================================================================
    // SwitchInfo

    void encode(const platform::switchboard::Switch &sw,
                platform::switchboard::protobuf::SwitchInfo *msg)
    {
        encode(*sw.spec(), msg->mutable_spec());
        encode(*sw.status(), msg->mutable_status());
    }

    //==========================================================================
    // SwitchMap

    void encode(const platform::switchboard::SwitchMap &map,
                platform::switchboard::protobuf::SwitchMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, sw] : map)
        {
            encode(*sw, &encoded_map[name]);
        }
    }

}  // namespace cc::protobuf
