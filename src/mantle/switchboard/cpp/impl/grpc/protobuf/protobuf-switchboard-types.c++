// -*- c++ -*-
//==============================================================================
/// @file protobuf-switchboard-types.c++
/// @brief conversions to/from Protocol Buffer messages in "switchboard.proto"
/// @author Tor Slettnes <tor@slett.net>
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
                cc::switchboard::State *proto)

    {
        *proto = static_cast<cc::switchboard::State>(native);
    }

    void decode(cc::switchboard::State proto,
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

    void encode(const std::string &name, cc::switchboard::SwitchIdentifier *msg)
    {
        msg->set_switch_name(name);
    }

    void decode(const cc::switchboard::SwitchIdentifier &msg, std::string &name)
    {
        name = msg.switch_name();
    }

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string> &names,
                cc::switchboard::SwitchIdentifiers *msg)
    {
        msg->mutable_switch_names()->Reserve(names.size());
        for (const std::string &name : names)
        {
            msg->add_switch_names(name);
        }
    }

    void decode(const cc::switchboard::SwitchIdentifiers &msg,
                std::vector<std::string> *names)
    {
        assign_to_vector(msg.switch_names(), names);
    }

    //==========================================================================
    // SwitchSet

    void encode(const switchboard::SwitchSet &set,
                cc::switchboard::SwitchIdentifiers *msg)
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
                cc::switchboard::Specification *msg)
    {
        msg->set_is_primary(spec.primary);
        encode(spec.descriptions, msg->mutable_descriptions());
        encode(spec.dependencies, msg->mutable_dependencies());
        encode(spec.interceptors, msg->mutable_interceptors());
    }

    void decode(const cc::switchboard::Specification &msg,
                const switchboard::ProviderRef &provider,
                switchboard::Specification *spec)
    {
        if (msg.has_is_primary())
        {
            spec->primary = msg.is_primary();
        }

        decode(msg.descriptions(), &spec->descriptions);
        decode(msg.dependencies(), provider, &spec->dependencies);
        decode(msg.interceptors(), &spec->interceptors);
    }

    //==========================================================================
    // Status

    void encode(const switchboard::Status &status,
                cc::switchboard::Status *msg)
    {
        msg->set_current_state(encoded<cc::switchboard::State>(status.current_state));
        msg->set_settled_state(encoded<cc::switchboard::State>(status.settled_state));
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

    void decode(const cc::switchboard::Status &msg,
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
                cc::switchboard::StatusMap *msg)
    {
        msg->clear_map();
        auto *map = msg->mutable_map();
        for (const auto &[id, status] : statusmap)
        {
            encode(*status, &(*map)[id]);
        }
    }

    void decode(const cc::switchboard::StatusMap &msg,
                switchboard::StatusMap *statusmap)
    {
        statusmap->clear();
        for (const auto &[id, protostatus] : msg.map())
        {
            decode_shared(protostatus, &(*statusmap)[id]);
        }
    }

    //==========================================================================
    // Description

    void encode(const switchboard::Description &description,
                cc::switchboard::Description *msg)
    {
        msg->set_text(description.text);

        auto &target_map = *msg->mutable_target_texts();
        for (const auto &[position, text] : description.target_texts)
        {
            target_map[position] = text;
        }

        auto &state_map = *msg->mutable_state_texts();
        for (const auto &[state, text] : description.state_texts)
        {
            state_map[state] = text;
        }
    }

    void decode(const cc::switchboard::Description &msg,
                switchboard::Description *description)
    {
        description->text = msg.text();
        for (const auto &item : msg.target_texts())
        {
            description->target_texts.insert(item);
        }

        for (const auto &[state, text] : msg.state_texts())
        {
            description->state_texts.insert_or_assign(
                static_cast<switchboard::State>(state),
                text);
        }
    }

    //==========================================================================
    // DescriptionMap

    void encode(const switchboard::DescriptionMap &map,
                cc::switchboard::DescriptionMap *msg)
    {
        auto &protomap = *msg->mutable_map();
        for (const auto &[language_code, description] : map)
        {
            encode(description, &protomap[language_code]);
        }
    }

    void decode(const cc::switchboard::DescriptionMap &msg,
                switchboard::DescriptionMap *map)
    {
        map->clear();
        for (const auto &[language, description] : msg.map())
        {
            decode(description, &(*map)[language]);
        }
    }

    //==========================================================================
    // DependencyPolarity

    void encode(switchboard::DependencyPolarity native,
                cc::switchboard::DependencyPolarity *proto)
    {
        *proto = static_cast<cc::switchboard::DependencyPolarity>(native);
    }

    void decode(cc::switchboard::DependencyPolarity proto,
                switchboard::DependencyPolarity *native)
    {
        *native = static_cast<switchboard::DependencyPolarity>(proto);
    }

    //==========================================================================
    // Dependency

    void encode(const switchboard::DependencyRef &native,
                cc::switchboard::DependencySpec *proto)
    {
        proto->set_trigger_states(native->trigger_states());
        proto->set_polarity(encoded<cc::switchboard::DependencyPolarity>(native->polarity()));
        proto->set_hard(native->hard());
        proto->set_sufficient(native->sufficient());
    }

    void decode(const cc::switchboard::DependencySpec &proto,
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
                cc::switchboard::DependencyMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, dep] : map)
        {
            encode(dep, &encoded_map[name]);
        }
    }

    void decode(const cc::switchboard::DependencyMap &msg,
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
                cc::switchboard::InterceptorSpec *proto)
    {
        proto->set_owner(native->owner());
        proto->set_state_transitions(native->state_transitions());
        proto->set_phase(encoded<cc::switchboard::InterceptorPhase>(native->phase()));
        proto->set_asynchronous(native->asynchronous());
        proto->set_rerun(native->rerun());
        proto->set_on_cancel(encoded<cc::switchboard::ExceptionHandling>(native->on_cancel()));
        proto->set_on_error(encoded<cc::switchboard::ExceptionHandling>(native->on_error()));
    }

    void decode(const cc::switchboard::InterceptorSpec &proto,
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
                cc::switchboard::InterceptorMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, ic] : map)
        {
            encode(ic, &encoded_map[name]);
        }
    }

    void decode(const cc::switchboard::InterceptorMap &proto,
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
                cc::switchboard::InterceptorPhase *proto)
    {
        *proto = static_cast<cc::switchboard::InterceptorPhase>(native);
    }

    void decode(cc::switchboard::InterceptorPhase proto,
                switchboard::InterceptorPhase *native)
    {
        *native = static_cast<switchboard::InterceptorPhase>(proto);
    }

    //==========================================================================
    // ExceptionHandling

    void encode(switchboard::ExceptionHandling native,
                cc::switchboard::ExceptionHandling *proto)
    {
        *proto = static_cast<cc::switchboard::ExceptionHandling>(native);
    }

    void decode(cc::switchboard::ExceptionHandling proto,
                switchboard::ExceptionHandling *native)
    {
        *native = static_cast<switchboard::ExceptionHandling>(proto);
    }

    //==========================================================================
    // SwitchInfo

    void encode(const switchboard::Switch &sw,
                cc::switchboard::SwitchInfo *msg)
    {
        encode(*sw.spec(), msg->mutable_spec());
        encode(*sw.status(), msg->mutable_status());
    }

    //==========================================================================
    // SwitchMap

    void encode(const switchboard::SwitchMap &map,
                cc::switchboard::SwitchMap *msg)
    {
        auto &encoded_map = *msg->mutable_map();
        for (const auto &[name, sw] : map)
        {
            encode(*sw, &encoded_map[name]);
        }
    }

}  // namespace protobuf
