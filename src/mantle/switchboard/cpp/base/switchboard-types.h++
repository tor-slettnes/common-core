// -*- c++ -*-
//==============================================================================
/// @file switchboard-types.h++
/// @brief Switchboard data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

// Shared utilities
#include "logging/logging.h++"
#include "types/valuemap.h++"
#include "types/symbolmap.h++"
#include "types/listable.h++"
#include "status/error.h++"

namespace switchboard
{
    define_log_scope("switch");

    //==========================================================================
    // Constants
    constexpr auto DEFAULT_LANGUAGE = "en";

    //==========================================================================
    // Data types & forward declarations

    class Provider;
    class Switch;
    struct Description;
    struct Specification;
    struct Status;
    class Dependency;
    class Interceptor;

    //==========================================================================
    // Switch states

    enum State
    {
        // Use powers of two, as values are used in bitmasks
        STATE_UNSET = 0x00,
        STATE_ACTIVATING = 0x01,
        STATE_ACTIVE = 0x02,
        STATE_DEACTIVATING = 0x04,
        STATE_INACTIVE = 0x08,
        STATE_FAILING = 0x10,
        STATE_FAILED = 0x20,
    };

    extern const core::types::SymbolMap<State> state_names;
    std::ostream &operator<<(std::ostream &stream, State state);
    std::istream &operator>>(std::istream &stream, State &state);

    //==========================================================================
    // Exception Handling

    enum ExceptionHandling
    {
        EH_DEFAULT,  // Default action for intercept
        EH_IGNORE,   // Ignore exceptions, proceed with state change
        EH_ABORT,    // Retain previous state, do not retain error
        EH_FAIL,     // Transition to error state.
        EH_REVERT    // Transition back to previous state, do not retain error
    };

    extern const core::types::SymbolMap<ExceptionHandling> exceptionhandling_names;
    std::ostream &operator<<(std::ostream &stream, ExceptionHandling eh);
    std::istream &operator>>(std::istream &stream, ExceptionHandling &eh);

    //==========================================================================
    // DependencyPolarity

    enum class DependencyPolarity
    {
        POSITIVE,  // Successor depends on predecessor being active
        NEGATIVE,  // Inverse; successor depends on predecessor being inactive
        TOGGLE     // Successor toggles in response to predecessor change

    };

    extern const core::types::SymbolMap<DependencyPolarity> depdir_names;
    std::ostream &operator<<(std::ostream &stream, DependencyPolarity eh);
    std::istream &operator>>(std::istream &stream, DependencyPolarity &eh);

    //==========================================================================
    // Type aliases

    using ProviderRef = std::shared_ptr<Provider>;

    using SwitchRef = std::shared_ptr<Switch>;
    using SwitchName = std::string;
    using SwitchNames = std::vector<SwitchName>;
    using SwitchSet = std::set<SwitchRef>;
    using SwitchMap = core::types::ValueMap<SwitchName, SwitchRef>;
    using CulpritsMap = std::unordered_map<SwitchRef, State>;
    using ErrorMap = std::unordered_map<SwitchRef, core::status::Error::ptr>;
    using LanguageCode = std::string;
    using DescriptionMap = core::types::ValueMap<LanguageCode, Description>;

    using SpecRef = std::shared_ptr<Specification>;
    using SpecMap = std::unordered_map<SwitchName, SpecRef>;

    using StatusRef = std::shared_ptr<Status>;
    using StatusMap = std::unordered_map<SwitchName, StatusRef>;

    using DependencyRef = std::shared_ptr<Dependency>;
    using DependencyMap = std::unordered_map<SwitchName, DependencyRef>;

    using InterceptorName = std::string;
    using InterceptorRef = std::shared_ptr<Interceptor>;
    using InterceptorMap = std::unordered_map<InterceptorName, InterceptorRef>;

    using StateMask = std::uint32_t;

    constexpr StateMask SETTLED_STATES = (STATE_ACTIVE | STATE_INACTIVE | STATE_FAILED);
    constexpr StateMask PENDING_STATES = (STATE_ACTIVATING | STATE_DEACTIVATING | STATE_FAILED);
    constexpr StateMask ACTIVATION_STATES = {STATE_ACTIVATING | STATE_DEACTIVATING};

    //==========================================================================
    // Description

    struct Description : public core::types::Listable
    {
        std::string text;
        std::unordered_map<bool, std::string> target_texts;
        std::unordered_map<State, std::string> state_texts;

        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

        friend bool operator==(const Description &lhs, const Description &rhs);
        friend bool operator!=(const Description &lhs, const Description &rhs);
    };


    //==========================================================================
    // Specification

    struct Specification : public core::types::Listable
    {
        bool primary = false;
        DescriptionMap descriptions;
        DependencyMap dependencies;
        InterceptorMap interceptors;

        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

        friend bool operator==(const Specification &lhs, const Specification &rhs);
        friend bool operator!=(const Specification &lhs, const Specification &rhs);
    };


    //==========================================================================
    // Status

    struct Status : public core::types::Listable
    {
        State current_state = STATE_UNSET;
        State settled_state = STATE_UNSET;
        bool active = false;   // Switch is currently in "ON" position
        bool pending = false;  // Switch is currently running interceptor(s)
        core::status::Error::ptr error;
        core::types::KeyValueMap attributes;

        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

        friend bool operator==(const Status &lhs, const Status &rhs);
        friend bool operator!=(const Status &lhs, const Status &rhs);
    };

}  // namespace switchboard
