// -*- c++ -*-
//==============================================================================
/// @file central-switch.h++
/// @brief Authoritative switch managed by SwitchboardCentral
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

    constexpr auto SETTING_SWITCH_NAME = "name";
    constexpr auto SETTING_SWITCH_ACTIVE = "active";
    constexpr auto SETTING_SWITCH_SETTLED_STATE = "settled_state";
    constexpr auto SETTING_SWITCH_STATE = "state";
    constexpr auto SETTING_SWITCH_ATTRIBUTES = "attributes";
    constexpr auto SETTING_SWITCH_ERROR = "error";
    constexpr auto SETTING_SPEC_PRIMARY = "primary";
    constexpr auto SETTING_SPEC_ALIASES = "aliases";
    constexpr auto SETTING_SPEC_DEPENDENCIES = "dependencies";
    constexpr auto SETTING_SPEC_INTERCEPTORS = "interceptors";
    constexpr auto SETTING_SPEC_LOCALIZATIONS = "localizations";
    constexpr auto SETTING_LOC_DESCRIPTION = "description";
    constexpr auto SETTING_LOC_STATE_TEXTS = "state texts";
    constexpr auto SETTING_LOC_ACTIVATE_TEXT = "activate text";
    constexpr auto SETTING_LOC_DEACTIVATE_TEXT = "deactivate text";
    constexpr auto SETTING_DEP_PREDECESSOR = "predecessor";
    constexpr auto SETTING_DEP_TRIGGERS = "trigger_states";
    constexpr auto SETTING_DEP_AUTOMATIC = "automatic";
    constexpr auto SETTING_DEP_DIRECTION = "polarity";
    constexpr auto SETTING_DEP_INVERTED = "inverted";
    constexpr auto SETTING_DEP_HARD = "hard";
    constexpr auto SETTING_DEP_SUFFICIENT = "sufficient";

    //==========================================================================
    /// @class CentralSwitch
    /// @brief Local Switch implementation, e.g. in Switchboard service or client.

    class CentralSwitch : public Switch,
                          public core::types::enable_create_shared<CentralSwitch>
    {
        using This = CentralSwitch;
        using Super = Switch;
        friend class Central;

    protected:
        using Switch::Switch;

    public:
        void set_spec(
            const Specification &spec) override;

    protected:
        bool add_dependency(
            const DependencyRef &dependency,
            bool allow_update,
            bool reevaluate = true) override;

        bool remove_dependency(
            SwitchName predecessor_name,
            bool reevaluate = true) override;

        bool add_interceptor(
            const InterceptorRef &interceptor,
            bool immediate = false) override;

        bool remove_interceptor(
            const InterceptorName &key) override;

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
            InvocationStyle invoke_interceptors = InvocationStyle::DEFAULT,
            CascadeStyle cascade_descendants = CascadeStyle::DEFAULT,
            bool reenter = false,
            ExceptionHandling on_cancel = EH_DEFAULT,
            ExceptionHandling on_error = EH_DEFAULT) override;

        core::types::KeyValueMap get_attributes(
            bool inherit) const override;

        bool set_attributes(
            const core::types::KeyValueMap &attributes,
            bool clear_existing) override;

    private:
        bool set_current_state(
            State state,
            InvocationStyle invoke_interceptors,
            CascadeStyle cascade_descendants = CascadeStyle::ASYNC,
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
            State state,
            InvocationStyle invoke_interceptors,
            CascadeStyle cascade_descendants);

        bool set_attributes_only(
            const core::types::KeyValueMap &attributes,
            bool clear_existing);

        void notify_spec();
        void notify_status();

    public:
        void import_spec(
            const core::types::KeyValueMap &declaration,
            bool replace_aliases,
            bool replace_localizations,
            bool replace_dependencies,
            bool replace_interceptors);

    private:
        Localization import_localization(
            const core::types::KeyValueMap &kvmap) const;

        DependencyRef import_dependency(
            const std::string &predecessor_name,
            const core::types::KeyValueMap &dep_map) const;

    public:
        void import_status(
            const core::types::KeyValueMap &status,
            bool replace_attributes,
            bool set_state,
            InvocationStyle invoke_interceptors);

    private:
        void export_spec(
            core::types::TaggedValueList *tvlist) const;

        void export_status(
            core::types::TaggedValueList *tvlist) const;

    private:
        static State transition_state(State target_state) noexcept;
        static bool target_position(State state, bool current) noexcept;

    private:
        SpecRef notified_spec;
        StatusRef notified_status;
        // std::recursive_mutex interceptor_mutex;
    };
}  // namespace switchboard
