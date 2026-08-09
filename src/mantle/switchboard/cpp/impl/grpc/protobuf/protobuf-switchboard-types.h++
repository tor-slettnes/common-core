// -*- c++ -*-
//==============================================================================
/// @file protobuf-switchboard-types.h++
/// @brief conversions to/from Protocol Buffer messages in "switchboard.proto"
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "cc/platform/switchboard/protobuf/switchboard_types.pb.h"  // generated from `switchboard.proto`

namespace cc::protobuf
{
    //==========================================================================
    // State

    void encode(platform::switchboard::State native,
                platform::switchboard::protobuf::State* proto);

    void decode(platform::switchboard::protobuf::State proto,
                platform::switchboard::State* native);

    //==========================================================================
    // State set

    void encode(const std::set<platform::switchboard::State>& states,
                google::protobuf::RepeatedField<int>* items);

    void decode(const google::protobuf::RepeatedField<int>& items,
                std::set<platform::switchboard::State>* states);

    //==========================================================================
    // SwitchIdentifier

    void encode(const std::string& name,
                platform::switchboard::protobuf::SwitchIdentifier* msg);

    void decode(const platform::switchboard::protobuf::SwitchIdentifier& msg,
                std::string* name);

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string>& names,
                platform::switchboard::protobuf::SwitchIdentifiers* msg);

    void decode(const platform::switchboard::protobuf::SwitchIdentifiers& msg,
                std::vector<std::string>* names);

    //==========================================================================
    // SwitchSet

    void encode(const platform::switchboard::SwitchSet& set,
                platform::switchboard::protobuf::SwitchIdentifiers* msg);

    //==========================================================================
    // SwitchSelection

    void encode(const platform::switchboard::SwitchSelection& pattern,
                platform::switchboard::protobuf::SwitchSelection* msg);

    void decode(const platform::switchboard::protobuf::SwitchSelection& msg,
                platform::switchboard::SwitchSelection* pattern);

    //==========================================================================
    // Specification

    void encode(const platform::switchboard::Specification& spec,
                platform::switchboard::protobuf::Specification* msg);

    void decode(const platform::switchboard::protobuf::Specification& msg,
                const platform::switchboard::ProviderRef& provider,
                platform::switchboard::Specification* spec);

    //==========================================================================
    // Status

    void encode(const platform::switchboard::Status& status,
                platform::switchboard::protobuf::Status* msg);

    void decode(const platform::switchboard::protobuf::Status& msg,
                platform::switchboard::Status* status);

    //==========================================================================
    // StatusMap

    void encode(const platform::switchboard::StatusMap& statusmap,
                platform::switchboard::protobuf::StatusMap* msg);

    void decode(const platform::switchboard::protobuf::StatusMap& msg,
                platform::switchboard::StatusMap* statusmap);

    //==========================================================================
    // ErrorMap

    void encode(const platform::switchboard::ErrorMap& errormap,
                platform::switchboard::protobuf::ErrorMap* msg);

    //==========================================================================
    // Aliases

    void encode(const std::set<platform::switchboard::SwitchName>& aliases,
                google::protobuf::RepeatedPtrField<std::string>* items);

    void decode(const google::protobuf::RepeatedPtrField<std::string>& items,
                std::set<platform::switchboard::SwitchName>* aliases);

    //==========================================================================
    // Localization

    void encode(const platform::switchboard::Localization& localization,
                platform::switchboard::protobuf::Localization* msg);

    void decode(const platform::switchboard::protobuf::Localization& msg,
                platform::switchboard::Localization* localization);

    //==========================================================================
    // LocalizationMap

    void encode(const platform::switchboard::LocalizationMap& map,
                platform::switchboard::protobuf::LocalizationMap* msg);

    void decode(const platform::switchboard::protobuf::LocalizationMap& msg,
                platform::switchboard::LocalizationMap* map);

    //==========================================================================
    // DependencyPolarity

    void encode(platform::switchboard::DependencyPolarity native,
                platform::switchboard::protobuf::DependencyPolarity* proto);

    void decode(platform::switchboard::protobuf::DependencyPolarity proto,
                platform::switchboard::DependencyPolarity* native);

    //==========================================================================
    // Dependency

    void encode(const platform::switchboard::DependencyRef& native,
                platform::switchboard::protobuf::Dependency* proto);

    void decode(const platform::switchboard::protobuf::Dependency& proto,
                const platform::switchboard::ProviderRef& provider,
                const platform::switchboard::SwitchName& predecessor_name,
                platform::switchboard::DependencyRef* native);

    //==========================================================================
    // DependencyMap

    void encode(const platform::switchboard::DependencyMap& map,
                platform::switchboard::protobuf::DependencyMap* msg);

    void decode(const platform::switchboard::protobuf::DependencyMap& msg,
                const platform::switchboard::ProviderRef& provider,
                platform::switchboard::DependencyMap* map);

    //==========================================================================
    // DependencyStatus

    void encode(const platform::switchboard::DependencyStatus& native,
                platform::switchboard::protobuf::DependencyStatus* proto);

    void decode(const platform::switchboard::protobuf::DependencyStatus& proto,
                const platform::switchboard::ProviderRef& provider,
                const platform::switchboard::SwitchName& predecessor_name,
                platform::switchboard::DependencyStatus* native);

    //==========================================================================
    // DependencyStatusMap

    void encode(const platform::switchboard::DependencyStatusMap& native,
                platform::switchboard::protobuf::DependencyStatusMap* proto);

    void decode(const platform::switchboard::protobuf::DependencyStatusMap& proto,
                const platform::switchboard::ProviderRef& provider,
                platform::switchboard::DependencyStatusMap* native);

    //==========================================================================
    // Interceptor

    void encode(const platform::switchboard::InterceptorRef& native,
                platform::switchboard::protobuf::InterceptorSpec* proto);

    void decode(const platform::switchboard::protobuf::InterceptorSpec& proto,
                const platform::switchboard::InterceptorName& name,
                const platform::switchboard::InterceptorOwner& owner,
                const platform::switchboard::Invocation& invocation,
                platform::switchboard::InterceptorRef* native);

    //==========================================================================
    // InterceptorMap

    void encode(const platform::switchboard::InterceptorMap& map,
                platform::switchboard::protobuf::InterceptorMap* msg);

    void decode(const platform::switchboard::protobuf::InterceptorMap& msg,
                platform::switchboard::InterceptorMap* map);

    //==========================================================================
    // InterceptorPhase

    void encode(platform::switchboard::InterceptorPhase native,
                platform::switchboard::protobuf::InterceptorPhase* proto);

    void decode(platform::switchboard::protobuf::InterceptorPhase proto,
                platform::switchboard::InterceptorPhase* native);

    //==========================================================================
    // ExceptionHandling

    void encode(platform::switchboard::ExceptionHandling native,
                platform::switchboard::protobuf::ExceptionHandling* proto);

    void decode(platform::switchboard::protobuf::ExceptionHandling proto,
                platform::switchboard::ExceptionHandling* native);

    //==========================================================================
    // InvocationStyle

    void encode(platform::switchboard::InvocationStyle native,
                platform::switchboard::protobuf::InvocationStyle* proto);

    void decode(platform::switchboard::protobuf::InvocationStyle proto,
                platform::switchboard::InvocationStyle* native);

    //==========================================================================
    // CascadeStyle

    void encode(platform::switchboard::CascadeStyle native,
                platform::switchboard::protobuf::CascadeStyle* proto);

    void decode(platform::switchboard::protobuf::CascadeStyle proto,
                platform::switchboard::CascadeStyle* native);

    //==========================================================================
    // SwitchInfo

    void encode(const platform::switchboard::Switch& sw,
                platform::switchboard::protobuf::SwitchInfo* msg);

    //==========================================================================
    // SwitchMap

    void encode(const platform::switchboard::SwitchMap& map,
                platform::switchboard::protobuf::SwitchMap* msg);

}  // namespace cc::protobuf
