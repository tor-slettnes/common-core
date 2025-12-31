// -*- c++ -*-
//==============================================================================
/// @file local-switch.h++
/// @brief Switch managed locally
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "types/create-shared.h++"

#include <future>
#include <mutex>

namespace switchboard
{
    using ThreadMap = std::unordered_map<SwitchRef, std::thread>;

    //==========================================================================
    /// @class LocalSwitch
    /// @brief Local Switch implementation, e.g. in Switchboard service or client.

    class LocalSwitch : public Switch,
                        public core::types::enable_create_shared<LocalSwitch>
    {
        using This = LocalSwitch;
        using Super = Switch;
        friend class SwitchboardStandalone;

    protected:
        using Switch::Switch;

    public:
        ~LocalSwitch();

        void set_spec(
            const Specification &spec) override;

    protected:
        bool add_dependency(
            const DependencyRef &dependency,
            bool allow_update,
            bool reevaluate) override;

        bool remove_dependency(
            SwitchName predecessor_name,
            bool reevaluate = true) override;

        bool add_interceptor(
            const InterceptorRef &interceptor,
            bool immediate = false) override;

        bool remove_interceptor(
            const InterceptorName &id) override;

        /// Update multiple specification values in one go.
        void update_spec(
            const std::optional<bool> &primary,
            const SwitchAliases &aliases,
            bool replace_aliases,
            const LocalizationMap &localizations,
            bool replace_localizations,
            const DependencyMap &dependencies,
            bool replace_dependencies,
            const InterceptorMap &interceptors,
            bool replace_interceptors,
            bool update_state) override;

        bool set_target(
            State target_state,
            const core::status::Error::ptr &error = {},
            const core::types::KeyValueMap &attributes = {},
            bool clear_existing = false,
            bool invoke_interceptors = true,
            bool trigger_descendants = true,
            bool reevaluate = false,
            ExceptionHandling on_cancel = EH_DEFAULT,
            ExceptionHandling on_error = EH_DEFAULT) override;

        bool set_attributes(
            const core::types::KeyValueMap &attributes,
            bool clear_existing) override;

    private:
        bool set_current_state(
            State state,
            bool invoke_interceptors = false,
            bool trigger_descendants = false,
            ExceptionHandling on_cancel = EH_DEFAULT,
            ExceptionHandling on_error = EH_DEFAULT);

        bool invoke_interceptors(
            State state,
            ExceptionHandling on_cancel = EH_DEFAULT,
            ExceptionHandling on_error = EH_DEFAULT);

        bool invoke_interceptors(
            State state,
            InterceptorPhase phase,
            ExceptionHandling on_cancel,
            ExceptionHandling on_error);

        bool handle_cancel(
            const std::unordered_set<InterceptorRef> &interceptors,
            State state,
            ExceptionHandling eh);

        bool handle_errors(
            const std::unordered_map<InterceptorRef, std::exception_ptr> &exceptions,
            State state,
            ExceptionHandling eh);

        bool handle_diversion(
            const core::status::Error::ptr &event,
            ExceptionHandling eh,
            ExceptionHandling eh_default);

        ThreadMap update_descendants(
            bool invoke_interceptors = true);

        void notify_spec();
        void notify_status();

        static State transition_state(State target_state) noexcept;
        static bool target_position(State state, bool current) noexcept;

    private:
        SpecRef notified_spec;
        StatusRef notified_status;
        // std::recursive_mutex interceptor_mutex;
    };
}  // namespace switchboard
