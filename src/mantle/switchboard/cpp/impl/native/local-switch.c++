// -*- c++ -*-
//==============================================================================
/// @file local-switch.c++
/// @brief Switch managed locally
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "local-switch.h++"
#include "status/exceptions.h++"

namespace switchboard
{

    LocalSwitch::~LocalSwitch()
    {
        signal_status.clear(this->name());
        signal_spec.clear(this->name());
    }

    bool LocalSwitch::add_dependency(const DependencyRef &dependency,
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

    bool LocalSwitch::remove_dependency(SwitchName predecessor_name,
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

    bool LocalSwitch::add_interceptor(const InterceptorRef &interceptor,
                                      bool immediate)
    {
        auto [it, inserted] = this->spec_ref->interceptors.insert_or_assign(
            interceptor->name(),
            interceptor);

        this->notify_spec();

        if (immediate)
        {
            State state = this->state();
            if (interceptor->applicable(state))
            {
                interceptor->invoke(this->shared_from_this(), state);
            }
        }

        return inserted;
    }

    bool LocalSwitch::remove_interceptor(
        const InterceptorName &id)
    {
        if (this->spec_ref->interceptors.erase(id))
        {
            this->notify_spec();
            return true;
        }
        else
        {
            return false;
        }
    }

    void LocalSwitch::set_spec(
        const Specification &spec)
    {
        Switch::set_spec(spec);
        this->notify_spec();
    }

    void LocalSwitch::update_spec(
        const std::optional<bool> &primary,
        const DescriptionMap &descriptions,
        bool replace_descriptions,
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

        if (replace_descriptions)
        {
            this->spec_ref->descriptions = descriptions;
        }
        else
        {
            this->spec_ref->descriptions.insert(descriptions.begin(), descriptions.end());
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

    bool LocalSwitch::set_target(
        State target_state,
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
        bool trigger_descendents,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        if (target_state == STATE_UNSET)
        {
            target_state = error ? STATE_FAILED
                                 : this->auto_state();
        }

        if (reevaluate || (this->state() != target_state))
        {
            bool proceed = true;
            State transition_state = This::transition_state(target_state);
            if (transition_state != STATE_UNSET)
            {
                logf_debug("Switch %r entering transition state %r", this->name(), transition_state);
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
                        trigger_descendents,
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
                    status->error = error;

                    if (clear_existing)
                    {
                        status->attributes = attributes;
                    }
                    else
                    {
                        status->attributes.update(attributes);
                    }
                }

                return this->set_current_state(
                    target_state,
                    invoke_interceptors,
                    trigger_descendents,
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

    bool LocalSwitch::set_attributes(const core::types::KeyValueMap &attributes,
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
            this->notify_status();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool LocalSwitch::set_current_state(
        State state,
        bool invoke_interceptors,
        bool trigger_descendents,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        bool success = true;
        Status &status = *this->status_ref;

        logf_debug("Switch %r: state=%s, invoke_interceptors=%b, trigger_descendents=%b",
                   this->name(),
                   state,
                   invoke_interceptors,
                   trigger_descendents);

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

        if (trigger_descendents)
        {
            ThreadMap threads = this->update_descendents(invoke_interceptors);

            // Ignore descendant resuls for now.
            for (auto &[sw, thread] : threads)
            {
                thread.detach();
            }
        }

        this->notify_status();
        return success;
    }

    bool LocalSwitch::invoke_interceptors(
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

    bool LocalSwitch::invoke_interceptors(
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
                logf_notice("Switch %r interceptor %r failed: %s",
                            this->name(),
                            ic->name(),
                            std::current_exception());
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

    bool LocalSwitch::handle_cancel(
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

    bool LocalSwitch::handle_errors(
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
                if (ic->on_error() > eh)
                {
                    eh = ic->on_error();
                    eh_source = "interceptor " + ic->name();
                    dominating_error = eptr;
                }
            }
        }

        logf_info(
            "Switch %r state %s change encountered %d errors; "
            "applying policy %s action based on %s",
            this->name(),
            state,
            exceptions.size(),
            eh,
            eh_source);

        return this->handle_diversion(core::exception::map_to_error(dominating_error),
                                      eh,
                                      EH_FAIL);
    }

    bool LocalSwitch::handle_diversion(const core::status::Error::ptr &error,
                                       ExceptionHandling eh,
                                       ExceptionHandling eh_default)
    {
        if (eh == EH_DEFAULT)
        {
            eh = eh_default;
        }

        logf_debug("Switch %r applying policy %s on exception: %s", this->name(), eh, *error);

        switch (eh == EH_DEFAULT ? eh_default : eh)
        {
        case EH_FAIL:
            this->set_error(error);
            return true;

        case EH_IGNORE:
            return false;

        case EH_ABORT:
            this->set_current_state(this->settled_state());
            return true;

        case EH_REVERT:
            this->set_target(
                this->settled_state(),  // target_state
                this->error(),          // error
                {},                     // attributes
                false,                  // clear_existing
                true,                   // invoke_interceptors
                false);                 // trigger_descendents
            return true;

        default:
            return true;
        }
    }

    ThreadMap LocalSwitch::update_descendents(bool invoke_interceptors)
    {
        ThreadMap threads;

        for (SwitchRef sw : this->get_successors())
        {
            if (DependencyRef dep = sw->get_dependency(this->name()))
            {
                if (dep->auto_trigger(this->state()))
                {
                    logf_trace("Switch %r updating descendent %r, interceptors=%b",
                               this->name(),
                               sw->name(),
                               invoke_interceptors);
                    this->notify_status();

                    threads.insert_or_assign(
                        sw,
                        std::thread(&Switch::set_auto,
                                    sw,
                                    core::types::KeyValueMap(),  // attributes
                                    false,                         // clear_existing
                                    invoke_interceptors,           // invoke_interceptors
                                    true,                          // trigger_descendents
                                    false,                         // reevaluate
                                    EH_DEFAULT,                    // on_cancel
                                    EH_DEFAULT));                  // on_error
                }
            }
        }
        return threads;
    }

    void LocalSwitch::notify_spec()
    {
        signal_spec.emit_if_changed(this->name(), *this->spec());
    }

    void LocalSwitch::notify_status()
    {
        signal_status.emit_if_changed(this->name(), *this->status());
    }

    State LocalSwitch::transition_state(State target_state) noexcept
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

    bool LocalSwitch::target_position(State state, bool current) noexcept
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
