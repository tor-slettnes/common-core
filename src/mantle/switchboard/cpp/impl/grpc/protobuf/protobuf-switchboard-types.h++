// -*- c++ -*-
//==============================================================================
/// @file protobuf-switchboard-types.h++
/// @brief conversions to/from Protocol Buffer messages in "switchboard.proto"
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "cc/switchboard/switchboard.pb.h"     // generated from `switchboard.proto`

namespace protobuf
{
    //==========================================================================
    // State

    void encode(switchboard::State native,
                cc::switchboard::State *proto);

    void decode(cc::switchboard::State proto,
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
                cc::switchboard::SwitchIdentifier *msg);

    void decode(const cc::switchboard::SwitchIdentifier &msg,
                std::string &name);

    //==========================================================================
    // SwitchIdentifiers

    void encode(const std::vector<std::string> &names,
                cc::switchboard::SwitchIdentifiers *msg);

    void decode(const cc::switchboard::SwitchIdentifiers &msg,
                std::vector<std::string> *names);

    //==========================================================================
    // SwitchSet

    void encode(const switchboard::SwitchSet &set,
                cc::switchboard::SwitchIdentifiers *msg);

    //==========================================================================
    // Specification

    void encode(const switchboard::Specification &spec,
                cc::switchboard::Specification *msg);

    void decode(const cc::switchboard::Specification &msg,
                const switchboard::ProviderRef &provider,
                switchboard::Specification *spec);

    //==========================================================================
    // Status

    void encode(const switchboard::Status &status,
                cc::switchboard::Status *msg);

    void decode(const cc::switchboard::Status &msg,
                switchboard::Status *status);

    //==========================================================================
    // StatusMap

    void encode(const switchboard::StatusMap &statusmap,
                cc::switchboard::StatusMap *msg);

    void decode(const cc::switchboard::StatusMap &msg,
                switchboard::StatusMap *statusmap);

    //==========================================================================
    // Localization

    void encode(const switchboard::Localization &localization,
                cc::switchboard::Localization *msg);

    void decode(const cc::switchboard::Localization &msg,
                switchboard::Localization *localization);

    //==========================================================================
    // LocalizationMap

    void encode(const switchboard::LocalizationMap &map,
                cc::switchboard::LocalizationMap *msg);

    void decode(const cc::switchboard::LocalizationMap &msg,
                switchboard::LocalizationMap *map);

    //==========================================================================
    // DependencyPolarity

    void encode(switchboard::DependencyPolarity native,
                cc::switchboard::DependencyPolarity *proto);

    void decode(cc::switchboard::DependencyPolarity proto,
                switchboard::DependencyPolarity *native);

    //==========================================================================
    // Dependency

    void encode(const switchboard::DependencyRef &native,
                cc::switchboard::Dependency *proto);

    void decode(const cc::switchboard::Dependency &proto,
                const switchboard::ProviderRef &provider,
                const std::string &predecessor_name,
                switchboard::DependencyRef *native);

    //==========================================================================
    // DependencyMap

    void encode(const switchboard::DependencyMap &map,
                cc::switchboard::DependencyMap *msg);

    void decode(const cc::switchboard::DependencyMap &msg,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyMap *map);

    //==========================================================================
    // Interceptor

    void encode(const switchboard::InterceptorRef &native,
                cc::switchboard::InterceptorSpec *proto);

    void decode(const cc::switchboard::InterceptorSpec &proto,
                const std::string &name,
                const switchboard::Invocation &invocation,
                switchboard::InterceptorRef *native);

    //==========================================================================
    // InterceptorMap

    void encode(const switchboard::InterceptorMap &map,
                cc::switchboard::InterceptorMap *msg);

    void decode(const cc::switchboard::InterceptorMap &msg,
                switchboard::InterceptorMap *map);

    //==========================================================================
    // InterceptorPhase

    void encode(switchboard::InterceptorPhase native,
                cc::switchboard::InterceptorPhase *proto);

    void decode(cc::switchboard::InterceptorPhase proto,
                switchboard::InterceptorPhase *native);

    //==========================================================================
    // ExceptionHandling

    void encode(switchboard::ExceptionHandling native,
                cc::switchboard::ExceptionHandling *proto);

    void decode(cc::switchboard::ExceptionHandling proto,
                switchboard::ExceptionHandling *native);

    //==========================================================================
    // SwitchInfo

    void encode(const switchboard::Switch &sw,
                cc::switchboard::SwitchInfo *msg);

    //==========================================================================
    // SwitchMap

    void encode(const switchboard::SwitchMap &map,
                cc::switchboard::SwitchMap *msg);

}  // namespace protobuf
