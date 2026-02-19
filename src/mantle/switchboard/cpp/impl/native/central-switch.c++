// -*- c++ -*-
//==============================================================================
/// @file central-switch.c++
/// @brief Authoritative switch managed by SwitchboardCentral
/// @author Tor Slettnes
//==============================================================================

#include "central-switch.h++"
#include "status/exceptions.h++"

namespace switchboard
{

    bool CentralSwitch::add_dependency(const DependencyRef &dependency,
                                       bool allow_update,
                                       bool reevaluate)
    {
        if (allow_update || !this->get_dependency(dependency->predecessor_name()))
        {
            auto [it, inserted] = this->spec_ref->dependencies.insert_or_assign(
                dependency->predecessor_name(),
                dependency);

            this->notify_spec();

            if (inserted && reevaluate)
            {
                this->set_auto();
            }
            return inserted;
        }
        else
        {
            return false;
        }
    }

    bool CentralSwitch::remove_dependency(SwitchName predecessor_name,
                                          bool reevaluate)
    {
        bool erased = this->spec_ref->dependencies.erase(predecessor_name);
        if (erased)
        {
            this->notify_spec();
            if (reevaluate)
            {
                this->set_auto();
            }
        }
        return erased;
    }

    bool CentralSwitch::add_interceptor(const InterceptorRef &interceptor,
                                        bool immediate)
    {
        auto [it, inserted] = this->spec_ref->interceptors.insert_or_assign(
            interceptor->name(),
            interceptor);

        this->notify_spec();

        if (immediate)
        {
            State state = this->state();
            if (interceptor->applicable(state) ||
                interceptor->applicable(This::transition_state(state)))
            {
                interceptor->invoke(this->shared_from_this(), state);
            }
        }

        return inserted;
    }

    bool CentralSwitch::remove_interceptor(
        const InterceptorName &name)
    {
        if (this->spec_ref->interceptors.erase(name))
        {
            this->notify_spec();
            return true;
        }
        else
        {
            return false;
        }
    }

    void CentralSwitch::set_spec(
        const Specification &spec)
    {
        Switch::set_spec(spec);
        this->notify_spec();
    }

    void CentralSwitch::update_spec(
        const std::optional<bool> &primary,
        const SwitchAliases &aliases,
        bool replace_aliases,
        const LocalizationMap &localizations,
        bool replace_localizations,
        const DependencyMap &dependencies,
        bool replace_dependencies,
        const InterceptorMap &interceptors,
        bool replace_interceptors,
        bool update_state)
    {
        if (primary.has_value())
        {
            this->spec_ref->primary = primary.value();
        }

        if (replace_aliases)
        {
            this->spec_ref->aliases = aliases;
        }
        else
        {
            this->spec_ref->aliases.insert(aliases.begin(), aliases.end());
        }

        if (replace_localizations)
        {
            this->spec_ref->localizations = localizations;
        }
        else
        {
            this->spec_ref->localizations.insert(localizations.begin(), localizations.end());
        }

        if (replace_dependencies)
        {
            this->spec_ref->dependencies = dependencies;
        }
        else
        {
            this->spec_ref->dependencies.insert(dependencies.begin(), dependencies.end());
        }

        if (replace_interceptors)
        {
            this->spec_ref->interceptors = interceptors;
        }
        else
        {
            this->spec_ref->interceptors.insert(interceptors.begin(), interceptors.end());
        }

        this->notify_spec();

        if (update_state)
        {
            this->set_auto();
        }
    }

    bool CentralSwitch::set_target(
        State target_state,
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
        CascadeStyle cascade_descendants,
        bool reenter,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        if (target_state == STATE_UNSET)
        {
            target_state = (error && *error) ? STATE_FAILED : this->auto_state();
        }

        if (reenter || (this->state() != target_state))
        {
            bool proceed = true;
            bool updated_status = false;
            if (State transition_state = This::transition_state(target_state))
            {
                logf_debug(
                    "Switch %r entering transition state %r",
                    this->name(),
                    transition_state);

                if (StatusRef status = this->status())
                {
                    status->error = error;
                    this->set_attributes_only(attributes, clear_existing);
                    updated_status = true;
                }

                if (this->state() == transition_state)
                {
                    proceed = !this->pending();
                }
                else if (this->interceptors().empty())
                {
                    proceed = true;
                }
                else
                {
                    proceed = this->set_current_state(
                        transition_state,
                        invoke_interceptors,
                        cascade_descendants,
                        on_cancel,
                        on_error);
                }
            }

            if (proceed)
            {
                logf_debug("Switch %r entering target state %r", this->name(), target_state);

                if (StatusRef status = this->status())
                {
                    status->active = this->target_position(target_state, this->active());
                    if (!updated_status)
                    {
                        status->error = error;
                        this->set_attributes_only(attributes, clear_existing);
                    }
                }

                return this->set_current_state(
                    target_state,
                    invoke_interceptors,
                    cascade_descendants,
                    EH_IGNORE,
                    EH_IGNORE);
            }
        }
        else if (!attributes.empty() ||
                 (clear_existing && (attributes != this->attributes())))
        {
            this->set_attributes(attributes, clear_existing);
        }
        return false;
    }

    bool CentralSwitch::set_attributes(const core::types::KeyValueMap &attributes,
                                       bool clear_existing)
    {
        if (this->set_attributes_only(attributes, clear_existing))
        {
            this->notify_status();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool CentralSwitch::set_current_state(
        State state,
        bool invoke_interceptors,
        CascadeStyle cascade_descendants,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        bool success = true;
        Status &status = *this->status_ref;

        logf_debug("Switch %r: state=%s, invoke_interceptors=%b, cascade_descendants=%s",
                   this->name(),
                   state,
                   invoke_interceptors,
                   cascade_descendants);

        status.current_state = state;
        if (Switch::is_settled(state))
        {
            status.settled_state = state;
        }

        if (invoke_interceptors)
        {
            status.pending = true;
            success = this->invoke_interceptors(state, on_cancel, on_error);
            status.pending = false;
        }

        this->notify_status();

        if (cascade_descendants != CascadeStyle::NONE)
        {
            ThreadMap threads = this->update_descendants(
                state,
                invoke_interceptors,
                cascade_descendants);

            if (cascade_descendants == CascadeStyle::ASYNC)
            {
                for (auto &[sw, thread] : threads)
                {
                    thread.detach();
                }
            }
            else
            {
                for (auto &[sw, thread] : threads)
                {
                    thread.join();
                }
            }
        }

        return success;
    }

    bool CentralSwitch::invoke_interceptors(
        State state,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        bool result = this->invoke_interceptors(state, PHASE_EARLY, on_cancel, on_error) &&
                      this->invoke_interceptors(state, PHASE_NORMAL, on_cancel, on_error) &&
                      this->invoke_interceptors(state, PHASE_LATE, on_cancel, on_error);
        // this->interceptor_mutex.unlock();
        return result;
    }

    bool CentralSwitch::invoke_interceptors(
        State state,
        InterceptorPhase phase,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        std::unordered_map<InterceptorRef, std::future<void>> results;
        std::unordered_set<InterceptorRef> invoked_interceptors;

        /// Launch interceptors in parallel
        for (const auto &[name, ic] : this->interceptors())
        {
            if (ic->applicable(state, phase))
            {
                // if (!this->interceptor_mutex.try_lock())
                // {
                //     logf_debug("Switch %r waiting to invoke %s %s interceptor %s",
                //                this->name(),
                //                phase,
                //                ic->asynchronous() ? "asynchronous" : "synchronous",
                //                name);
                //     this->interceptor_mutex.lock();
                // }
                this->notify_status();

                logf_debug("Switch %r invoking %s %s interceptor %r",
                           this->name(),
                           phase,
                           ic->asynchronous() ? "asynchronous" : "synchronous",
                           name);

                std::future<void> future_result = ic->invoke(this->shared_from_this(), state);
                if (!ic->asynchronous())
                {
                    results.insert_or_assign(ic, std::move(future_result));
                }
                invoked_interceptors.insert(ic);
            }
        }

        /// Wait for interceptors, collect any errors
        std::unordered_map<InterceptorRef, std::exception_ptr> errors;
        for (auto &[ic, result] : results)
        {
            try
            {
                logf_debug("Waiting for switch %r interceptor %r",
                           this->name(),
                           ic->name());
                result.get();
            }
            catch (...)
            {
                errors.insert_or_assign(ic, std::current_exception());
            }
        }

        if (this->state() != state)
        {
            return !this->handle_cancel(invoked_interceptors, state, on_cancel);
        }
        else if (errors.size())
        {
            return !this->handle_errors(errors, state, on_error);
        }
        else
        {
            return true;
        }
    }

    bool CentralSwitch::handle_cancel(
        const std::unordered_set<InterceptorRef> &interceptors,
        State state,
        ExceptionHandling eh)
    {
        std::string eh_source = "explicit argument";
        if (eh == EH_DEFAULT)
        {
            eh_source = "default behavior";
            for (const InterceptorRef &ic : interceptors)
            {
                if (ic->on_cancel() > eh)
                {
                    eh = ic->on_cancel();
                    eh_source = "interceptor " + ic->name();
                }
            }
        }
        logf_info("Cancelling %r state %s change; applying policy %s based on %s",
                  this->name(),
                  state,
                  eh,
                  eh_source);

        return this->handle_diversion(std::make_shared<core::exception::Cancelled>(),
                                      eh,
                                      EH_IGNORE);
    }

    bool CentralSwitch::handle_errors(
        const std::unordered_map<InterceptorRef, std::exception_ptr> &exceptions,
        State state,
        ExceptionHandling eh)
    {
        std::string eh_source = "explicit argument";
        std::exception_ptr dominating_error;
        if (eh == EH_DEFAULT)
        {
            eh_source = "default behavior";
            for (const auto &[ic, eptr] : exceptions)
            {
                if (!dominating_error || (ic->on_error() > eh))
                {
                    eh = ic->on_error();
                    eh_source = "interceptor " + ic->name();
                    dominating_error = eptr;
                }
            }
        }

        logf_debug(
            "Switch %r state %s change encountered %d errors; "
            "applying policy %s action based on %s",
            this->name(),
            state,
            exceptions.size(),
            eh,
            eh_source);

        if (dominating_error)
        {
            return this->handle_diversion(core::exception::map_to_error(dominating_error),
                                          eh,
                                          EH_FAIL);
        }
        else
        {
            return false;
        }
    }

    bool CentralSwitch::handle_diversion(const core::status::Error::ptr &error,
                                         ExceptionHandling eh,
                                         ExceptionHandling eh_default)
    {
        if (eh == EH_DEFAULT)
        {
            eh = eh_default;
        }

        logf_debug("Switch %r applying policy %s from exception: %s",
                   this->name(),
                   eh,
                   *error);

        switch (eh == EH_DEFAULT ? eh_default : eh)
        {
        case EH_FAIL:
            this->set_error(error);
            return true;

        case EH_IGNORE:
            return false;

        case EH_ABORT:
            this->set_current_state(
                this->settled_state(),  // state
                false,                  // invoke_interceptors,
                CascadeStyle::NONE);    // cascade_descendants,
            return true;

        case EH_REVERT:
            this->set_target(
                this->settled_state(),  // target_state
                this->error(),          // error
                {},                     // attributes
                false,                  // clear_existing
                true,                   // invoke_interceptors
                CascadeStyle::NONE);    // cascade_descendants
            return true;

        default:
            return true;
        }
    }

    ThreadMap CentralSwitch::update_descendants(
        State state,
        bool invoke_interceptors,
        CascadeStyle cascade_descendants)
    {
        ThreadMap threads;

        if (cascade_descendants == CascadeStyle::WAIT_DIRECT)
        {
            // If we are waiting only for these direct descentant, do not
            // cascade further to indirect ones.
            cascade_descendants = CascadeStyle::ASYNC;
        }

        for (SwitchRef sw : this->get_successors())
        {
            if (DependencyRef dep = sw->get_dependency(this->name()))
            {
                if (dep->auto_trigger(state))
                {
                    logf_trace("Switch %r state %s updating descendant %r, interceptors=%b",
                               this->name(),
                               state,
                               sw->name(),
                               invoke_interceptors);
                    this->notify_status();

                    threads.insert_or_assign(
                        sw,
                        std::thread(&Switch::set_auto,
                                    sw,
                                    core::types::KeyValueMap(),  // attributes
                                    false,                       // clear_existing
                                    invoke_interceptors,         // invoke_interceptors
                                    cascade_descendants,         // cascade_descendants
                                    false,                       // reenter
                                    EH_DEFAULT,                  // on_cancel
                                    EH_DEFAULT));                // on_error
                }
            }
        }
        return threads;
    }

    bool CentralSwitch::set_attributes_only(
        const core::types::KeyValueMap &attributes,
        bool clear_existing)
    {
        if (StatusRef status = this->status())
        {
            if (clear_existing)
            {
                status->attributes = attributes;
            }
            else
            {
                status->attributes.update(attributes);
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    void CentralSwitch::notify_spec()
    {
        signal_spec.emit_if_changed(this->name(), *this->spec());
    }

    void CentralSwitch::notify_status()
    {
        signal_status.emit_if_changed(this->name(), *this->status());
    }

    State CentralSwitch::transition_state(State target_state) noexcept
    {
        switch (target_state)
        {
        case STATE_ACTIVE:
            return STATE_ACTIVATING;

        case STATE_INACTIVE:
            return STATE_DEACTIVATING;

        case STATE_FAILED:
            return STATE_FAILING;

        default:
            return STATE_UNSET;
        }
    }

    bool CentralSwitch::target_position(State state, bool current) noexcept
    {
        switch (state)
        {
        case STATE_INACTIVE:
            return false;
        case STATE_ACTIVE:
            return true;
        default:
            return current;
        }
    }

}  // namespace switchboard
