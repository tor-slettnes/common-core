// -*- c++ -*-
//==============================================================================
/// @file protobuf-switchboard-types.c++
/// @brief conversions to/from Protocol Buffer messages in "switchboard.proto"
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-switchboard-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // State

    void encode(switchboard::State native,
                cc::platform::switchboard::protobuf::State *proto)

    {
        *proto = static_cast<cc::platform::switchboard::protobuf::State>(native);
    }

    void decode(cc::platform::switchboard::protobuf::State proto,
                switchboard::State *native)
    {
        *native = static_cast<switchboard::State>(proto);
    }

    //==========================================================================
    // State set

    void encode(const std::set<switchboard::State> &states,
                google::protobuf::RepeatedField<int> *items)
    {
        items->Clear();
        items->Reserve(states.size());
        for (switchboard::State state : states)
        {
            items->Add(state);
        }
    }

    void decode(const google::protobuf::RepeatedField<int> &items,
                std::set<switchboard::State> *states)
    {
        states->clear();
        for (int state : items)
        {
            states->insert(static_cast<switchboard::State>(state));
        }
    }

    //==========================================================================
    // SwitchIdentifier

    void encode(const std::string &name, cc::platform::switchboard::protobuf::SwitchIdentifier *msg)
    {
        msg->set_switch_name(name);
    }

    void decode(const cc::platform::switchboard::protobuf::SwitchIdentifier &msg, std::string &name)
    {
        name = msg.switch_name();
    }

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string> &names,
                cc::platform::switchboard::protobuf::SwitchIdentifiers *msg)
    {
        msg->mutable_switch_names()->Reserve(names.size());
        for (const std::string &name : names)
        {
            msg->add_switch_names(name);
        }
    }

    void decode(const cc::platform::switchboard::protobuf::SwitchIdentifiers &msg,
                std::vector<std::string> *names)
    {
        assign_to_vector(msg.switch_names(), names);
    }

    //==========================================================================
    // SwitchSet

    void encode(const switchboard::SwitchSet &set,
                cc::platform::switchboard::protobuf::SwitchIdentifiers *msg)
    {
        msg->mutable_switch_names()->Reserve(set.size());
        for (const auto &sw : set)
        {
            msg->add_switch_names(sw->name());
        }
    }

    //==========================================================================
    // Specification

    void encode(const switchboard::Specification &spec,
                cc::platform::switchboard::protobuf::Specification *msg)
    {
        msg->set_is_primary(spec.primary);
        encode(spec.aliases, msg->mutable_aliases());
        encode(spec.localizations, msg->mutable_localizations());
        encode(spec.dependencies, msg->mutable_dependencies());
        encode(spec.interceptors, msg->mutable_interceptors());
    }

    void decode(const cc::platform::switchboard::protobuf::Specification &msg,
                const switchboard::ProviderRef &provider,
                switchboard::Specification *spec)
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

    void encode(const switchboard::Status &status,
                cc::platform::switchboard::protobuf::Status *msg)
    {
        msg->set_current_state(encoded<cc::platform::switchboard::protobuf::State>(status.current_state));
        msg->set_settled_state(encoded<cc::platform::switchboard::protobuf::State>(status.settled_state));
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

    void decode(const cc::platform::switchboard::protobuf::Status &msg,
                switchboard::Status *status)
    {
        status->current_state = decoded<switchboard::State>(msg.current_state());
        status->settled_state = decoded<switchboard::State>(msg.settled_state());
        status->active = msg.active();
        status->pending = msg.pending();
        if (msg.has_error())
        {
            decode_shared(msg.error(), &status->error);
        }
        decode(msg.attributes(), &status->attributes);
    }

    //==========================================================================
    // StatusMap

    void encode(const switchboard::StatusMap &statusmap,
                cc::platform::switchboard::protobuf::StatusMap *msg)
    {
        msg->clear_map();
        auto *map = msg->mutable_map();
        for (const auto &[id, status] : statusmap)
        {
            encode(*status, &(*map)[id]);
        }
    }

    void decode(const cc::platform::switchboard::protobuf::StatusMap &msg,
                switchboard::StatusMap *statusmap)
    {
        statusmap->clear();
        for (const auto &[id, protostatus] : msg.map())
        {
            decode_shared(protostatus, &(*statusmap)[id]);
        }
    }

    //==========================================================================
    // Aliases

    void encode(const std::set<switchboard::SwitchName> &aliases,
                google::protobuf::RepeatedPtrField<std::string> *items)
    {
        items->Clear();
        items->Reserve(aliases.size());
        for (const switchboard::SwitchName &alias: aliases)
        {
            items->Add()->assign(alias);
        }
    }

    void decode(const google::protobuf::RepeatedPtrField<std::string> &items,
                std::set<switchboard::SwitchName> *aliases)
    {
        aliases->clear();
        aliases->insert(items.begin(), items.end());
    }

    //==========================================================================
    // Localization

    void encode(const switchboard::Localization &localization,
                cc::platform::switchboard::protobuf::Localization *msg)
    {
        msg->set_description(localization.description);

        auto &target_map = *msg->mutable_target_texts();
        for (const auto &[position, text] : localization.target_texts)
        {
            target_map[position] = text;
        }

        auto &state_map = *msg->mutable_state_texts();
        for (const auto &[state, text] : localization.state_texts)
        {
            state_map[state] = text;
        }
    }

    void decode(const cc::platform::switchboard::protobuf::Localization &msg,
                switchboard::Localization *localization)
    {
        localization->description = msg.description();
        for (const auto &item : msg.target_texts())
        {
            localization->target_texts.insert(item);
        }

        for (const auto &[state, text] : msg.state_texts())
        {
            localization->state_texts.insert_or_assign(
                static_cast<switchboard::State>(state),
                text);
        }
    }

    //==========================================================================
    // LocalizationMap

    void encode(const switchboard::LocalizationMap &map,
                cc::platform::switchboard::protobuf::LocalizationMap *msg)
    {
        auto &protomap = *msg->mutable_map();
        for (const auto &[language_code, localization] : map)
        {
            encode(localization, &protomap[language_code]);
        }
    }

    void decode(const cc::platform::switchboard::protobuf::LocalizationMap &msg,
                switchboard::LocalizationMap *map)
    {
        map->clear();
        for (const auto &[language, localization] : msg.map())
        {
            decode(localization, &(*map)[language]);
        }
    }

    //==========================================================================
    // DependencyPolarity

    void encode(switchboard::DependencyPolarity native,
                cc::platform::switchboard::protobuf::DependencyPolarity *proto)
    {
        *proto = static_cast<cc::platform::switchboard::protobuf::DependencyPolarity>(native);
    }

    void decode(cc::platform::switchboard::protobuf::DependencyPolarity proto,
                switchboard::DependencyPolarity *native)
    {
        *native = static_cast<switchboard::DependencyPolarity>(proto);
    }

    //==========================================================================
    // Dependency

    void encode(const switchboard::DependencyRef &native,
                cc::platform::switchboard::protobuf::Dependency *proto)
    {
        proto->set_trigger_states(native->trigger_states());
        proto->set_polarity(encoded<cc::platform::switchboard::protobuf::DependencyPolarity>(native->polarity()));
        proto->set_hard(native->hard());
        proto->set_sufficient(native->sufficient());
    }

    void decode(const cc::platform::switchboard::protobuf::Dependency &proto,
                const switchboard::ProviderRef &provider,
                const std::string &predecessor_name,
                switchboard::DependencyRef *native)
    {
        *native = switchboard::Dependency::create_shared(
            provider,
            predecessor_name,
            proto.trigger_states(),
            decoded<switchboard::DependencyPolarity>(proto.polarity()),
            proto.hard(),
            proto.sufficient());
    }

    //==========================================================================
    // DependencyMap

    void encode(const switchboard::DependencyMap &map,
                cc::platform::switchboard::protobuf::DependencyMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, dep] : map)
        {
            encode(dep, &encoded_map[name]);
        }
    }

    void decode(const cc::platform::switchboard::protobuf::DependencyMap &msg,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyMap *map)
    {
        for (const auto &[predecessor_name, spec] : msg.map())
        {
            decode(spec, provider, predecessor_name, &(*map)[predecessor_name]);
        }
    }

    //==========================================================================
    // Interceptor

    void encode(const switchboard::InterceptorRef &native,
                cc::platform::switchboard::protobuf::InterceptorSpec *proto)
    {
        proto->set_owner(native->owner());
        proto->set_state_transitions(native->state_transitions());
        proto->set_phase(encoded<cc::platform::switchboard::protobuf::InterceptorPhase>(native->phase()));
        proto->set_asynchronous(native->asynchronous());
        proto->set_rerun(native->rerun());
        proto->set_on_cancel(encoded<cc::platform::switchboard::protobuf::ExceptionHandling>(native->on_cancel()));
        proto->set_on_error(encoded<cc::platform::switchboard::protobuf::ExceptionHandling>(native->on_error()));
    }

    void decode(const cc::platform::switchboard::protobuf::InterceptorSpec &proto,
                const std::string &name,
                const switchboard::Invocation &invocation,
                switchboard::InterceptorRef *native)
    {
        *native = switchboard::Interceptor::create_shared(
            name,
            proto.owner(),
            invocation,
            proto.state_transitions(),
            decoded<switchboard::InterceptorPhase>(proto.phase()),
            proto.asynchronous(),
            proto.rerun(),
            decoded<switchboard::ExceptionHandling>(proto.on_cancel()),
            decoded<switchboard::ExceptionHandling>(proto.on_error()));
    }

    //==========================================================================
    // InterceptorMap

    void encode(const switchboard::InterceptorMap &map,
                cc::platform::switchboard::protobuf::InterceptorMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, ic] : map)
        {
            encode(ic, &encoded_map[name]);
        }
    }

    void decode(const cc::platform::switchboard::protobuf::InterceptorMap &proto,
                switchboard::InterceptorMap *native)
    {
        for (const auto &[name, spec] : proto.map())
        {
            decode(spec, name, {}, &(*native)[name]);
        }
    }

    //==========================================================================
    // InterceptorPhase

    void encode(switchboard::InterceptorPhase native,
                cc::platform::switchboard::protobuf::InterceptorPhase *proto)
    {
        *proto = static_cast<cc::platform::switchboard::protobuf::InterceptorPhase>(native);
    }

    void decode(cc::platform::switchboard::protobuf::InterceptorPhase proto,
                switchboard::InterceptorPhase *native)
    {
        *native = static_cast<switchboard::InterceptorPhase>(proto);
    }

    //==========================================================================
    // ExceptionHandling

    void encode(switchboard::ExceptionHandling native,
                cc::platform::switchboard::protobuf::ExceptionHandling *proto)
    {
        *proto = static_cast<cc::platform::switchboard::protobuf::ExceptionHandling>(native);
    }

    void decode(cc::platform::switchboard::protobuf::ExceptionHandling proto,
                switchboard::ExceptionHandling *native)
    {
        *native = static_cast<switchboard::ExceptionHandling>(proto);
    }

    //==========================================================================
    // SwitchInfo

    void encode(const switchboard::Switch &sw,
                cc::platform::switchboard::protobuf::SwitchInfo *msg)
    {
        encode(*sw.spec(), msg->mutable_spec());
        encode(*sw.status(), msg->mutable_status());
    }

    //==========================================================================
    // SwitchMap

    void encode(const switchboard::SwitchMap &map,
                cc::platform::switchboard::protobuf::SwitchMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, sw] : map)
        {
            encode(*sw, &encoded_map[name]);
        }
    }

}  // namespace protobuf
