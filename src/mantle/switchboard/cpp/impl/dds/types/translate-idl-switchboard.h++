/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-switchboard.h++
/// @brief Encode/decode routines for switchboard IDL types
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-types.hpp"  // generated from `switchboard-types.idl`

#include "switchboard.h++"
#include "translate-idl-variant.h++"
#include "translate-idl-eventtypes.h++"

namespace cc::idl
{
    // CC::Switchboard::ExceptionHandling
    void encode(cc::platform::switchboard::ExceptionHandling native,
                CC::Switchboard::ExceptionHandling *idl);

    void decode(CC::Switchboard::ExceptionHandling idl,
                cc::platform::switchboard::ExceptionHandling *native);

    // CC::Switchboard::InterceptorPhase
    void encode(cc::platform::switchboard::InterceptorPhase native,
                CC::Switchboard::InterceptorPhase *idl);

    void decode(CC::Switchboard::InterceptorPhase idl,
                cc::platform::switchboard::InterceptorPhase *native);

    // CC::Switchboard::InvocationStyle
    void encode(cc::platform::switchboard::InvocationStyle native,
                CC::Switchboard::InvocationStyle *idl);

    void decode(CC::Switchboard::InvocationStyle idl,
                cc::platform::switchboard::InvocationStyle *native);

    // CC::Switchboard::CascadeStyle
    void encode(cc::platform::switchboard::CascadeStyle native,
                CC::Switchboard::CascadeStyle *idl);

    void decode(CC::Switchboard::CascadeStyle idl,
                cc::platform::switchboard::CascadeStyle *native);

    // CC::Switchboard::State
    void encode(cc::platform::switchboard::State native,
                CC::Switchboard::State *idl);

    void decode(CC::Switchboard::State idl,
                cc::platform::switchboard::State *native);

    // CC::Switchboard::AliasList
    void encode(cc::platform::switchboard::SwitchAliases native,
                CC::Switchboard::NameList *idl);

    void decode(CC::Switchboard::NameList idl,
                cc::platform::switchboard::SwitchAliases *native);

    // CC::Switchboard::SwitchSelection
    void encode(const cc::platform::switchboard::SwitchSelection &native,
                CC::Switchboard::SwitchSelection *idl);

    void decode(const CC::Switchboard::SwitchSelection &idl,
                cc::platform::switchboard::SwitchSelection *native);

    // CC::Switchboard::Localization
    void encode(const cc::platform::switchboard::LanguageCode &language_code,
                const cc::platform::switchboard::Localization &native,
                CC::Switchboard::Localization *idl);

    void decode(const CC::Switchboard::Localization &idl,
                cc::platform::switchboard::LanguageCode *language_code,
                cc::platform::switchboard::Localization *localization);

    // CC::Switchboard::LocalizationList
    void encode(const cc::platform::switchboard::LocalizationMap &native,
                CC::Switchboard::LocalizationList *idl);

    void decode(const CC::Switchboard::LocalizationList &idl,
                cc::platform::switchboard::LocalizationMap *native);

    // CC::Switchboard::DependencyPolarity
    void encode(const cc::platform::switchboard::DependencyPolarity &native,
                CC::Switchboard::DependencyPolarity *idl);

    void decode(const CC::Switchboard::DependencyPolarity &idl,
                cc::platform::switchboard::DependencyPolarity *native);

    // CC::Switchboard::Dependency
    void encode(const cc::platform::switchboard::DependencyRef &native,
                CC::Switchboard::Dependency *idl);

    void decode(const CC::Switchboard::Dependency &idl,
                const cc::platform::switchboard::ProviderRef &provider,
                cc::platform::switchboard::DependencyRef *native);

    // CC::Switchboard::DependencyList
    void encode(const cc::platform::switchboard::DependencyMap &native,
                CC::Switchboard::DependencyList *idl);

    void decode(const CC::Switchboard::DependencyList &idl,
                const cc::platform::switchboard::ProviderRef &provider,
                cc::platform::switchboard::DependencyMap *native);

    // CC::Switchboard::Interceptor
    void encode(const cc::platform::switchboard::InterceptorRef &native,
                CC::Switchboard::Interceptor *idl);

    void decode(const CC::Switchboard::Interceptor &idl,
                const std::optional<cc::platform::switchboard::InterceptorOwner> &owner,
                const cc::platform::switchboard::Invocation &invocation,
                cc::platform::switchboard::InterceptorRef *native);

    // CC::Switchboard::InterceptorList
    void encode(const cc::platform::switchboard::InterceptorMap &native,
                CC::Switchboard::InterceptorList *idl);

    void decode(const CC::Switchboard::InterceptorList &idl,
                cc::platform::switchboard::InterceptorMap *native);

    // CC::Switchboard::Specification
    void encode(const cc::platform::switchboard::SwitchName &name,
                const cc::platform::switchboard::Specification &native,
                CC::Switchboard::Specification *idl);

    void decode(const CC::Switchboard::Specification &idl,
                const cc::platform::switchboard::ProviderRef &provider,
                cc::platform::switchboard::SwitchName *name,
                cc::platform::switchboard::Specification *native);

    // CC::Switchboard::Status
    void encode(const cc::platform::switchboard::SwitchName &name,
                const cc::platform::switchboard::Status &status,
                CC::Switchboard::Status *idl);

    void decode(const CC::Switchboard::Status &idl,
                cc::platform::switchboard::SwitchName *name,
                cc::platform::switchboard::Status *status);

    // CC::Switchboard::Switch
    void encode(const cc::platform::switchboard::Switch &sw,
                CC::Switchboard::Switch *idl);

    // CC::Switchboard::SwitchList
    void encode(const cc::platform::switchboard::SwitchMap &native,
                CC::Switchboard::SwitchList *idl);

}  // namespace cc::idl
