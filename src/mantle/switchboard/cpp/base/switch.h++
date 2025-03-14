// -*- c++ -*-
//==============================================================================
/// @file switch.h++
/// @brief Switch Base class
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard-types.h++"
#include "switch-dependency.h++"
#include "switch-interceptor.h++"
#include "types/streamable.h++"
#include <memory>

namespace switchboard
{
    //==========================================================================
    // @class Switch

    class Switch : public core::types::Streamable,
                   public std::enable_shared_from_this<Switch>
    {
        friend class Switchboard;

    protected:
        Switch(const SwitchName &name,
               const std::shared_ptr<switchboard::Provider> &provider);

        void to_stream(std::ostream &stream) const override;

    public:
        /// return the name of this switch
        const SwitchName &name() const noexcept;

        //return a reference to the Switchboard provider to which this switch belongs
        std::shared_ptr<Provider> provider() const;

        /// Return a map of direct dependencies for this switch
        const DependencyMap &dependencies() const noexcept;

        /// Return a specific dependency, nor an empty pointer if not found.
        DependencyRef get_dependency(const SwitchName &switch_name) const noexcept;

        /// Add a dependency. The switch may change its state as a result.
        virtual bool add_dependency(
            const DependencyRef &dependency,
            bool allow_update = true,
            bool reevaluate = true) = 0;

        /// Remove an existing dependency. The switch may change its state as a result.
        virtual bool remove_dependency(
            SwitchName predecessor_name,
            bool reevaluate = true) = 0;

        /// Return a map of immediate predecessors of this switch
        SwitchSet get_predecessors() const noexcept;

        /// Return references to immediate successors of this switch
        SwitchSet get_successors() const noexcept;

        /// Return references to all direct and indirect predecessors of this switch
        SwitchSet get_ancestors() const noexcept;

        /// Return references to all direct and indirect successors of this switch
        SwitchSet get_descendents() const noexcept;

        /// Return a map of this switch's interceptors
        const InterceptorMap &interceptors() const noexcept;

        /// Obtain an intereceptor by name
        /// @param[in] name
        ///    Interceptor name
        /// @param[in] required
        ///    Throw `exception::NotFound` if not found.
        /// @return
        ///    Interceptor reference.
        InterceptorRef get_interceptor(
            const InterceptorName &name,
            bool required = false) const;

        /// Add an interceptor with a callback handler to be invoked following
        /// an applicable state change.
        ///
        /// @param[in] interceptor
        ///      Interceptor to be invoked after each matching state change.
        /// @param[in] immediate
        ///      If the interceptor's triggers include this switch's current
        ///      state, invoke it immediately.
        /// @return
        ///     `true` if the interceptor was added, `false` if the name already existed.
        virtual bool add_interceptor(
            const InterceptorRef &interceptor,
            bool immediate = false) = 0;

        /// Remove an interceptor with a callback handler to be invoked
        /// following a state change.
        /// @return
        ///     `true` if the interceptor was removed, `false` if it did not exist.
        virtual bool remove_interceptor(
            const InterceptorName &name) = 0;

        /// Get current specification.
        /// @note
        ///     Subclasses may override implementation
        /// @return
        ///    Specification
        virtual SpecRef spec() const;

        /// @brief
        ///    Update specification.
        /// @param[in] spec
        ///    Specification
        /// @note
        ///    This does not propagate changes to descendents.
        virtual void set_spec(
            const Specification &spec);

        /// @brief Update multiple specification values in one go.
        /// @param[in] primary
        ///    If provided, updates `primary` flag to indicate whether a search
        ///    for culprits should stop with this switch rather than recurse
        ///    further into its ancestors.
        /// @param[in] descriptions
        ///    A map of languages and corresponding human readable texts
        ///    describing this switch and its activities.
        /// @param[in] replace_descriptions
        ///    Replace instead of merging into existing descriptions
        /// @param[in] dependencies
        ///    A map of predecessor switches and corresonding dependency
        ///    declarations.
        /// @param[in] replace_dependencies
        ///    Replace instead of merging into existing dependencies
        /// @param[in] interceptors
        ///    A map of interceptor names and corresponding declarations
        /// @param[in] replace_interceptors
        ///    Replace instead of merging into existing interceptors
        /// @param[in] update_state
        ///    Update the state of this switch after removing/adding
        ///    dependencies

        virtual void update_spec(
            const std::optional<bool> &primary,
            const DescriptionMap &descriptions,
            bool replace_descriptions,
            const DependencyMap &dependencies,
            bool replace_dependencies,
            const InterceptorMap &interceptors,
            bool replace_interceptors,
            bool update_state) = 0;

        /// Indicate whether switch is primary, i.e. do not descend
        /// into dependencies when looking for culprits.
        void set_primary(bool primary);

        /// Indicate whether switch is primary, i.e. do not descend
        /// into dependencies when looking for culprits.
        bool primary() const noexcept;

        /// Describe the switch
        void set_descriptions(const DescriptionMap &descriptions);

        /// Return the switch's descriptions in all available languages
        DescriptionMap descriptions() const noexcept;

        /// Return the switch's description in a specific language
        std::optional<std::string> description(
            const LanguageCode &language_code = DEFAULT_LANGUAGE) const noexcept;

        /// Return a textual explanation of an action, e.g.:
        /// get_switch("Cloud:Available").target_text(true) -> "connect to cloud"
        std::string target_text(
            bool active,
            const LanguageCode &lanugage_code = DEFAULT_LANGUAGE) const noexcept;

        /// Return a textual explanation of an reason, e.g.:
        /// @code
        ///    get_switch("Network:Available").state_text(STATE_INACTIVE)
        ///         -> "no network is available"
        /// @endcode
        std::string state_text(State state,
                               const std::string &language_code = DEFAULT_LANGUAGE) const noexcept;

        /// Get current status.
        /// @note
        ///     Subclasses may override implementation
        /// @return
        ///    Status reference
        virtual StatusRef status() const;

        /// Get current position
        /// @return
        ///    `true` if  switch is active, `false` if inactive
        operator bool() const noexcept;

        /// Get current position
        /// @return
        ///    `true` if  switch is active, `false` if inactive
        bool active() const noexcept;

        // Indicate whether the switch is currently active
        bool failed() const noexcept;

        /// Get current/target position corresponding to the provided state
        /// @param[in] state
        /// @return
        ///    `true` if the state is activating or active
        static bool is_active_target(State state) noexcept;

        /// Indicate whether the specified state is a settled state
        static bool is_settled(State state);

        // Indicate whether the switch is in a settled state
        bool settled() const noexcept;

        // Indicate whether the switch is currently running any interceptors.
        bool pending() const noexcept;

        // Get current state
        State state() const noexcept;

        // // Get current state
        // State current_state() const noexcept;

        // /// @brief Get target state.
        // /// @return
        // ///    Expected target state if change is in progress, otherwise
        // ///    `STATE_UNSET`.
        // State target_state() const noexcept;

        /// @brief Get settled state
        State settled_state() const noexcept;

        /// @brief Get target state associated with the current state
        State target_state() const noexcept;

        /// @brief Get target state associated with the provided state
        static State target_state(State current) noexcept;

        /// @brief Get state associated with the provided active flag
        static State target_state(bool active) noexcept;

        /// @brief Get automatic state value
        State auto_state() const noexcept;

        /// Transition to the specified target state.
        ///
        /// @param[in] target_state
        ///    Target state. Normally this is one of the "settled" states
        ///    (STATE_ACTIVE, STATE_INACTIVE, or STATE_FAILED), in which case
        ///    the switch will first change to the corresponding pending state
        ///    (STATE_ACTIVATING, STATE_DEACTIVATING, STATE_FAILING), triggering
        ///    any associated descendent updates and interceptor invocations.
        ///    If not provided (STATE_UNSET), then
        ///    * if `error` is provided is inferred as STATE_FAILED,
        ///    * otherwise, is derived from the switch's dependencies.
        ///
        /// @param[in] error
        ///    An event reference, containing any error associated with this
        ///    switch.  Ignored if target_state is provided but is not nne of
        ///    STATE_FAILING or STATE_FAILED.
        ///
        /// @param[in] attributes
        ///    Arbitrary key/value pairs associated with the switch's
        ///    target state.
        ///
        /// @param[in] clear_existing
        ///    Clear existing attributes
        ///
        /// @param[in] invoke_interceptors
        ///    Invoke any associated interceptors (handlers) after each
        ///    state transition.
        ///
        /// @param[in] trigger_descendents
        ///    Trigger any direct descendents to reevaluate their state based
        ///    on this and its other dependencies after each state transition.
        ///
        /// @param[in] reevaluate
        ///    Make the state transition even if the switch is already in the
        ///    desired target state.
        ///
        /// @param[in] on_cancel
        ///    Action to take if the transition is cancelled (for instance, by
        ///    initiating another transition whie running interceptors)
        ///
        /// @param[in] on_error
        ///    Action to take if an error is encountered while running
        ///    interceptors following a `pending` state transition.
        ///
        /// @return
        ///    `true` if the switch state was updated (either because the
        ///    target is different from the current state, or because the
        ///    `reevaluate` argument was set), otherwise `false`.
        ///
        /// Setting the switch position involves the following steps:
        ///
        ///   (a) Depending on the target, the switch's current state changes
        ///       to one of `STATE_DEACTIVATING`, `STATE_ACTIVATING`, or
        ///       `STATE_FAILING`.  Any existing event data is cleared, and in
        ///       the case of STATE_FAILING/STATE_FAILED, is replaced with the
        ///       provided data. Additionally, the supplied attributes are
        ///       retained as part of the switch's status.  A `signal_state`
        ///       update is emitted, but only if (b) and/or (c) applies.
        ///
        ///   (b) If `invoke_interceptors` is true, any corresponding interceptors
        ///       are invoked.
        ///
        ///   (c) If `trigger_descendents` is true, any direct descendents with
        ///       triggers for the corresponding state are reevaluated.
        ///
        ///   (d) Any interceptors that were invoked in step (b) whose
        ///       `asynchronous` flag is not set are allowed to complete.
        ///
        ///   (e) If any interceptor encountered an error, or if its target is
        ///       again changed while the transition is underway, then depending
        ///       on that interceptor's `on_error` or `on_cancel` attribute,
        ///       respectively, the transition to the target state may be
        ///       allowed to complete, aborted, reverted to its previous state,
        ///       or diverted to STATE_FAILED as if via `set_error()`.
        ///
        ///   (f) Otherwise, if the target state is one of the settled states
        ///       `STATE_INACTIVE`, `STATE_ACTIVE` or `STATE_FAILED`,
        ///       the switch's current state is changed accordingly, and steps
        ///       (b) through (d) are repated for this state.
        ///
        ///   Thus, note that it is possible for the switch to be diverted to
        ///   `STATE_FAILED` either before or after the intended operation is
        ///   complete.

        virtual bool set_target(
            State target_state,
            const core::status::Error::ptr &error = {},
            const core::types::KeyValueMap &attributes = {},
            bool clear_existing = false,
            bool invoke_interceptors = true,
            bool trigger_descendents = true,
            bool reevaluate = false,
            ExceptionHandling on_cancel = EH_DEFAULT,
            ExceptionHandling on_error = EH_DEFAULT) = 0;

        /// Change to the specified position: false=off, true=on.
        /// @sa set_target().
        bool set_active(bool active,
                        const core::types::KeyValueMap &attributes = {},
                        bool clear_existing = false,
                        bool invoke_interceptors = true,
                        bool trigger_descendents = true,
                        bool reevaluate = false,
                        ExceptionHandling on_cancel = EH_DEFAULT,
                        ExceptionHandling on_error = EH_DEFAULT);

        /// Set the switch to the FAILED state, with an associated error.
        /// @sa set_target();
        bool set_error(const core::status::Error::ptr &error,
                       const core::types::KeyValueMap &attributes = {},
                       bool clear_existing = false,
                       bool invoke_interceptors = true,
                       bool trigger_descendents = true,
                       bool reevaluate = false,
                       ExceptionHandling on_cancel = EH_DEFAULT,
                       ExceptionHandling on_error = EH_DEFAULT);

        /// Turn the switch on or off based on its dependencies.
        /// If the state has no dependencies it remains unchanged.
        /// @sa set_target()
        bool set_auto(const core::types::KeyValueMap &attributes = {},
                      bool clear_existing = false,
                      bool invoke_interceptors = true,
                      bool trigger_descendents = true,
                      bool reevaluate = false,
                      ExceptionHandling on_cancel = EH_DEFAULT,
                      ExceptionHandling on_error = EH_DEFAULT);

        /// Get any (non-transient) error directly associated with this switch,
        /// either injected or encountered during a state transition.
        /// This may be empty even if the state is `STATE_FAILED`, in which
        /// case the state is inherited from one of its dependencies.
        core::status::Error::ptr error() const noexcept;

        /// Return a map of upstrem switches with directly associated errors
        /// that cause this switch's current/target state to be STATE_FAILED.
        /// (If this switch is in a different state, this map is empty.)
        /// @sa culprits()
        ErrorMap errors() const noexcept;

        /// Get all key/value attributes associated with this switch
        const core::types::KeyValueMap &attributes() const noexcept;

        /// Get specific attribute
        core::types::Value attribute(const std::string &key) const;

        /// Set arbitrary attributes on this switch.
        virtual bool set_attributes(
            const core::types::KeyValueMap &attributes,
            bool clear_existing = false) = 0;

        /// Set an arbitrary attribute on this switch.
        void set_attribute(
            const std::string &name,
            const core::types::Value &value);

        /// Return ancestors whose current state prevent this switch from
        /// becoming active, either because they are in a FAILED state
        /// or due to a (direct or indirect) conflict.
        CulpritsMap culprits(bool expected = true) const noexcept;

    protected:
        SwitchName name_;
        std::weak_ptr<switchboard::Provider> provider_;
        SpecRef spec_ref;
        StatusRef status_ref;
    };

}  // namespace switchboard
