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

    void encode(switchboard::State native,
                cc::platform::switchboard::protobuf::State *proto);

    void decode(cc::platform::switchboard::protobuf::State proto,
                switchboard::State *native);

    //==========================================================================
    // State set

    void encode(const std::set<switchboard::State> &states,
                google::protobuf::RepeatedField<int> *items);

    void decode(const google::protobuf::RepeatedField<int> &items,
                std::set<switchboard::State> *states);

    //==========================================================================
    // SwitchIdentifier

    void encode(const std::string &name,
                cc::platform::switchboard::protobuf::SwitchIdentifier *msg);

    void decode(const cc::platform::switchboard::protobuf::SwitchIdentifier &msg,
                std::string *name);

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string> &names,
                cc::platform::switchboard::protobuf::SwitchIdentifiers *msg);

    void decode(const cc::platform::switchboard::protobuf::SwitchIdentifiers &msg,
                std::vector<std::string> *names);

    //==========================================================================
    // SwitchSet

    void encode(const switchboard::SwitchSet &set,
                cc::platform::switchboard::protobuf::SwitchIdentifiers *msg);

    //==========================================================================
    // SwitchSelection

    void encode(const switchboard::SwitchSelection &pattern,
                cc::platform::switchboard::protobuf::SwitchSelection *msg);

    void decode(const cc::platform::switchboard::protobuf::SwitchSelection &msg,
                switchboard::SwitchSelection *pattern);


    //==========================================================================
    // Specification

    void encode(const switchboard::Specification &spec,
                cc::platform::switchboard::protobuf::Specification *msg);

    void decode(const cc::platform::switchboard::protobuf::Specification &msg,
                const switchboard::ProviderRef &provider,
                switchboard::Specification *spec);

    //==========================================================================
    // Status

    void encode(const switchboard::Status &status,
                cc::platform::switchboard::protobuf::Status *msg);

    void decode(const cc::platform::switchboard::protobuf::Status &msg,
                switchboard::Status *status);

    //==========================================================================
    // StatusMap

    void encode(const switchboard::StatusMap &statusmap,
                cc::platform::switchboard::protobuf::StatusMap *msg);

    void decode(const cc::platform::switchboard::protobuf::StatusMap &msg,
                switchboard::StatusMap *statusmap);

    //==========================================================================
    // ErrorMap

    void encode(const switchboard::ErrorMap &errormap,
                cc::platform::switchboard::protobuf::ErrorMap *msg);


    //==========================================================================
    // Aliases

    void encode(const std::set<switchboard::SwitchName> &aliases,
                google::protobuf::RepeatedPtrField<std::string> *items);

    void decode(const google::protobuf::RepeatedPtrField<std::string> &items,
                std::set<switchboard::SwitchName> *aliases);

    //==========================================================================
    // Localization

    void encode(const switchboard::Localization &localization,
                cc::platform::switchboard::protobuf::Localization *msg);

    void decode(const cc::platform::switchboard::protobuf::Localization &msg,
                switchboard::Localization *localization);

    //==========================================================================
    // LocalizationMap

    void encode(const switchboard::LocalizationMap &map,
                cc::platform::switchboard::protobuf::LocalizationMap *msg);

    void decode(const cc::platform::switchboard::protobuf::LocalizationMap &msg,
                switchboard::LocalizationMap *map);

    //==========================================================================
    // DependencyPolarity

    void encode(switchboard::DependencyPolarity native,
                cc::platform::switchboard::protobuf::DependencyPolarity *proto);

    void decode(cc::platform::switchboard::protobuf::DependencyPolarity proto,
                switchboard::DependencyPolarity *native);

    //==========================================================================
    // Dependency

    void encode(const switchboard::DependencyRef &native,
                cc::platform::switchboard::protobuf::Dependency *proto);

    void decode(const cc::platform::switchboard::protobuf::Dependency &proto,
                const switchboard::ProviderRef &provider,
                const switchboard::SwitchName &predecessor_name,
                switchboard::DependencyRef *native);

    //==========================================================================
    // DependencyMap

    void encode(const switchboard::DependencyMap &map,
                cc::platform::switchboard::protobuf::DependencyMap *msg);

    void decode(const cc::platform::switchboard::protobuf::DependencyMap &msg,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyMap *map);

    //==========================================================================
    // DependencyStatus

    void encode(const switchboard::DependencyStatus &native,
                cc::platform::switchboard::protobuf::DependencyStatus *proto);

    void decode(const cc::platform::switchboard::protobuf::DependencyStatus &proto,
                const switchboard::ProviderRef &provider,
                const switchboard::SwitchName &predecessor_name,
                switchboard::DependencyStatus *native);


    //==========================================================================
    // DependencyStatusMap

    void encode(const switchboard::DependencyStatusMap &native,
                cc::platform::switchboard::protobuf::DependencyStatusMap *proto);

    void decode(const cc::platform::switchboard::protobuf::DependencyStatusMap &proto,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyStatusMap *native);

    //==========================================================================
    // Interceptor

    void encode(const switchboard::InterceptorRef &native,
                cc::platform::switchboard::protobuf::InterceptorSpec *proto);

    void decode(const cc::platform::switchboard::protobuf::InterceptorSpec &proto,
                const switchboard::InterceptorName &name,
                const switchboard::InterceptorOwner &owner,
                const switchboard::Invocation &invocation,
                switchboard::InterceptorRef *native);

    //==========================================================================
    // InterceptorMap

    void encode(const switchboard::InterceptorMap &map,
                cc::platform::switchboard::protobuf::InterceptorMap *msg);

    void decode(const cc::platform::switchboard::protobuf::InterceptorMap &msg,
                switchboard::InterceptorMap *map);

    //==========================================================================
    // InterceptorPhase

    void encode(switchboard::InterceptorPhase native,
                cc::platform::switchboard::protobuf::InterceptorPhase *proto);

    void decode(cc::platform::switchboard::protobuf::InterceptorPhase proto,
                switchboard::InterceptorPhase *native);

    //==========================================================================
    // ExceptionHandling

    void encode(switchboard::ExceptionHandling native,
                cc::platform::switchboard::protobuf::ExceptionHandling *proto);

    void decode(cc::platform::switchboard::protobuf::ExceptionHandling proto,
                switchboard::ExceptionHandling *native);

    //==========================================================================
    // InvocationStyle

    void encode(switchboard::InvocationStyle native,
                cc::platform::switchboard::protobuf::InvocationStyle *proto);

    void decode(cc::platform::switchboard::protobuf::InvocationStyle proto,
                switchboard::InvocationStyle *native);

    //==========================================================================
    // CascadeStyle

    void encode(switchboard::CascadeStyle native,
                cc::platform::switchboard::protobuf::CascadeStyle *proto);

    void decode(cc::platform::switchboard::protobuf::CascadeStyle proto,
                switchboard::CascadeStyle *native);

    //==========================================================================
    // SwitchInfo

    void encode(const switchboard::Switch &sw,
                cc::platform::switchboard::protobuf::SwitchInfo *msg);

    //==========================================================================
    // SwitchMap

    void encode(const switchboard::SwitchMap &map,
                cc::platform::switchboard::protobuf::SwitchMap *msg);

}  // namespace cc::protobuf
