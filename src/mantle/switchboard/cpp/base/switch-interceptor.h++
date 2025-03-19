// -*- c++ -*-
//==============================================================================
/// @file switch-interceptor.h++
/// @brief Task that is invoked in response to a switch state change
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

// Shared utilities
#include "switchboard-types.h++"
#include "types/listable.h++"
#include "types/create-shared.h++"

// STL
#include <string>
#include <future>

namespace switchboard
{
    //==========================================================================
    // Interceptor Phase

    enum InterceptorPhase
    {
        PHASE_EARLY,   // Invoke before main interceptors
        PHASE_NORMAL,  // Invoke as part of main control
        PHASE_LATE     // Invoke after completing main interceptors
    };

    extern const core::types::SymbolMap<InterceptorPhase> interceptor_phase_names;
    std::ostream &operator<<(std::ostream &stream, InterceptorPhase ep);
    std::istream &operator>>(std::istream &stream, InterceptorPhase &ep);

    //==========================================================================
    // Invocation signatures
    using Invocation = std::function<void(SwitchRef sw, State state)>;

    //==========================================================================
    /// @class Interceptor
    /// @brief Task that is invoked in response to a switch state change

    class Interceptor : public core::types::Listable,
                        public core::types::enable_create_shared<Interceptor>
    {
        using This = Interceptor;

    protected:
        Interceptor(const std::string &name,
                    const std::string &owner = {},
                    const Invocation &invocation = {},
                    StateMask state_transitions = ACTIVATION_STATES,
                    InterceptorPhase phase = PHASE_NORMAL,
                    bool asynchronous = false,
                    bool rerun = false,
                    ExceptionHandling on_cancel = EH_ABORT,
                    ExceptionHandling on_error = EH_FAIL,
                    core::status::Level log_failure_level = core::status::Level::NOTICE);

    public:
        // Accessor methods
        std::string name() const;
        std::string owner() const;
        Invocation invocation() const;
        StateMask state_transitions() const;
        InterceptorPhase phase() const;
        bool asynchronous() const;
        bool rerun() const;
        ExceptionHandling on_cancel() const;
        ExceptionHandling on_error() const;

    public:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
        void set_invocation(const Invocation &invocation);

    public:
        bool applicable(State state);
        bool applicable(State state, InterceptorPhase phase);

        std::future<void> invoke(SwitchRef sw, State state);
        void wait(std::future<void> &result);

    private:
        void run_invocation(SwitchRef sw, State state);

    public:
        std::string name_;
        std::string owner_;
        Invocation invocation_;
        StateMask state_transitions_;
        InterceptorPhase phase_;
        bool asynchronous_;
        bool rerun_;
        ExceptionHandling on_cancel_;
        ExceptionHandling on_error_;
        core::status::Level log_failure_level_;
    };
}  // namespace switchboard
