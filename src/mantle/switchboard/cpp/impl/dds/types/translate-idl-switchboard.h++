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

namespace idl
{
    // CC::Switchboard::ExceptionHandling
    void encode(switchboard::ExceptionHandling native,
                CC::Switchboard::ExceptionHandling *idl);

    void decode(CC::Switchboard::ExceptionHandling idl,
                switchboard::ExceptionHandling *native);

    // CC::Switchboard::InterceptorPhase
    void encode(switchboard::InterceptorPhase native,
                CC::Switchboard::InterceptorPhase *idl);

    void decode(CC::Switchboard::InterceptorPhase idl,
                switchboard::InterceptorPhase *native);

    // CC::Switchboard::State
    void encode(switchboard::State native,
                CC::Switchboard::State *idl);

    void decode(CC::Switchboard::State idl,
                switchboard::State *native);


    // CC::Switchboard::AliasList
    void encode(switchboard::SwitchAliases native,
                CC::Switchboard::AliasList *idl);

    void decode(CC::Switchboard::AliasList idl,
                switchboard::SwitchAliases *native);



    // CC::Switchboard::Localization
    void encode(const switchboard::LanguageCode &language_code,
                const switchboard::Localization &native,
                CC::Switchboard::Localization *idl);

    void decode(const CC::Switchboard::Localization &idl,
                switchboard::LanguageCode *language_code,
                switchboard::Localization *localization);

    // CC::Switchboard::LocalizationList
    void encode(const switchboard::LocalizationMap &native,
                CC::Switchboard::LocalizationList *idl);

    void decode(const CC::Switchboard::LocalizationList &idl,
                switchboard::LocalizationMap *native);

    // CC::Switchboard::DependencyPolarity
    void encode(const switchboard::DependencyPolarity &native,
                CC::Switchboard::DependencyPolarity *idl);

    void decode(const CC::Switchboard::DependencyPolarity &idl,
                switchboard::DependencyPolarity *native);

    // CC::Switchboard::Dependency
    void encode(const switchboard::DependencyRef &native,
                CC::Switchboard::Dependency *idl);

    void decode(const CC::Switchboard::Dependency &idl,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyRef *native);

    // CC::Switchboard::DependencyList
    void encode(const switchboard::DependencyMap &native,
                CC::Switchboard::DependencyList *idl);

    void decode(const CC::Switchboard::DependencyList &idl,
                const switchboard::ProviderRef &provider,
                switchboard::DependencyMap *native);

    // CC::Switchboard::Interceptor
    void encode(const switchboard::InterceptorRef &native,
                CC::Switchboard::Interceptor *idl);

    void decode(const CC::Switchboard::Interceptor &idl,
                const switchboard::Invocation &invocation,
                switchboard::InterceptorRef *native);

    // CC::Switchboard::InterceptorList
    void encode(const switchboard::InterceptorMap &native,
                CC::Switchboard::InterceptorList *idl);

    void decode(const CC::Switchboard::InterceptorList &idl,
                switchboard::InterceptorMap *native);

    // // CC::Switchboard::InterceptorID
    // void encode(const switchboard::SwitchName &switch_name,
    //             const switchboard::InterceptorName &interceptor_name,
    //             CC::Switchboard::InterceptorID *idl);

    // void decode(const CC::Switchboard::InterceptorID &idl,
    //             switchboard::SwitchName *switch_name,
    //             switchboard::InterceptorName *interceptor_name);

    // CC::Switchboard::Specification
    void encode(const switchboard::SwitchName &name,
                const switchboard::Specification &native,
                CC::Switchboard::Specification *idl);

    void decode(const CC::Switchboard::Specification &idl,
                const switchboard::ProviderRef &provider,
                switchboard::SwitchName *name,
                switchboard::Specification *native);

    // CC::Switchboard::Status
    void encode(const switchboard::SwitchName &name,
                const switchboard::Status &status,
                CC::Switchboard::Status *idl);

    void decode(const CC::Switchboard::Status &idl,
                switchboard::SwitchName *name,
                switchboard::Status *status);


    // CC::Switchboard::Switch
    void encode(const switchboard::Switch &sw,
                CC::Switchboard::Switch *idl);

    // CC::Switchboard::SwitchList
    void encode(const switchboard::SwitchMap &native,
                CC::Switchboard::SwitchList *idl);

}  // namespace idl
