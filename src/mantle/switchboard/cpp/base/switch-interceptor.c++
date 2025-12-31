// -*- c++ -*-
//==============================================================================
/// @file switch-interceptor.c++
/// @brief Task that is invoked in response to a switch state change
/// @author Tor Slettnes
//==============================================================================

#include "switch-interceptor.h++"
#include "switch.h++"
#include "string/format.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

#include <functional>

namespace switchboard
{
    //==========================================================================
    // Interceptor Phase

    const core::types::SymbolMap<InterceptorPhase> interceptor_phase_names = {
        {PHASE_EARLY, "EARLY"},
        {PHASE_NORMAL, "NORMAL"},
        {PHASE_LATE, "LATE"},
    };

    std::ostream &operator<<(std::ostream &stream, InterceptorPhase ep)
    {
        return interceptor_phase_names.to_stream(stream, ep);
    }

    std::istream &operator>>(std::istream &stream, InterceptorPhase &ep)
    {
        return interceptor_phase_names.from_stream(stream, &ep, PHASE_NORMAL);
    }

    //==========================================================================
    // Interceptor

    Interceptor::Interceptor(const std::string &name,
                             const std::string &owner,
                             const Invocation &invocation,
                             StateMask state_transitions,
                             InterceptorPhase phase,
                             bool asynchronous,
                             bool rerun,
                             ExceptionHandling on_cancel,
                             ExceptionHandling on_error,
                             core::status::Level log_failure_level)
        : name_(name),
          owner_(owner),
          invocation_(invocation),
          state_transitions_(state_transitions),
          phase_(phase),
          asynchronous_(asynchronous),
          rerun_(rerun),
          on_cancel_(on_cancel),
          on_error_(on_error),
          log_failure_level_(log_failure_level)
    {
    }

    // Accessor methods
    std::string Interceptor::name() const
    {
        return this->name_;
    }

    std::string Interceptor::owner() const
    {
        return this->owner_;
    }

    Invocation Interceptor::invocation() const
    {
        return this->invocation_;
    }

    StateMask Interceptor::state_transitions() const
    {
        return this->state_transitions_;
    }

    InterceptorPhase Interceptor::phase() const
    {
        return this->phase_;
    }

    bool Interceptor::asynchronous() const
    {
        return this->asynchronous_;
    }

    bool Interceptor::rerun() const
    {
        return this->rerun_;
    }

    ExceptionHandling Interceptor::on_cancel() const
    {
        return this->on_cancel_;
    }

    ExceptionHandling Interceptor::on_error() const
    {
        return this->on_error_;
    }

    void Interceptor::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"name", this->name()},
            {"owner", this->owner()},
            {"state_transitions", this->state_transitions()},
            {"asynchronous", this->asynchronous()},
            {"rerun", this->rerun()},
            {"on_cancel", core::str::convert_from(this->on_cancel())},
            {"on_error", core::str::convert_from(this->on_error())},
        });
    }

    bool Interceptor::applicable(State state)
    {
        return (this->state_transitions() & static_cast<StateMask>(state)) != 0;
    }

    bool Interceptor::applicable(State state, InterceptorPhase phase)
    {
        return this->applicable(state) && (phase == this->phase());
    }

    void Interceptor::set_invocation(const Invocation &invocation)
    {
        this->invocation_ = invocation;
    }

    std::future<void> Interceptor::invoke(
        SwitchRef sw,
        State state)
    {
        std::packaged_task<void()> task(std::bind(&This::run_invocation, this, sw, state));
        std::future<void> future_result = task.get_future();
        std::thread thread(std::move(task));
        thread.detach();
        return future_result;
    }

    void Interceptor::wait(std::future<void> &result)
    {
        if (this->asynchronous())
        {
            logf_debug("Interceptor %r is running asynchronously",
                       this->name());
        }
        else
        {
            logf_info("Waiting for interceptor %r", this->name());
            result.get();
        }
    }

    void Interceptor::run_invocation(SwitchRef sw, State state)
    {
        logf_debug("Invoking switch %r interceptor %r for state %s",
                   sw->name(),
                   this->name(),
                   state);
        try
        {
            this->invocation_(sw, state);
            logf_debug("Switch %r interceptor %r completed",
                       sw->name(),
                       this->name());
        }
        catch (...)
        {
            logf_message(this->log_failure_level_,
                         "Switch %r interceptor %r failed: %s",
                         sw->name(),
                         this->name(),
                         std::current_exception());
            std::rethrow_exception(std::current_exception());
        }
    }

}  // namespace switchboard
