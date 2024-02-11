/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.c++
/// @brief Simple non-buffered adaptation of Signal/Slot pattern
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "signaltemplate.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace cc::signal
{
    define_log_scope("signal");

    //==========================================================================
    /// \class BaseSignal
    /// \brief Abstract base for Signal<T> and MappingSignal<T>

    BaseSignal::BaseSignal(const std::string &name, bool caching)
        : name_(name),
          caching_(caching)
    {
        // logf_trace("%s: created", this->name_);
    }

    void BaseSignal::set_caching(bool caching)
    {
        this->caching_ = caching;
    }

    std::string BaseSignal::name()
    {
        return this->name_;
    }

    void BaseSignal::safe_invoke(const std::string &receiver,
                                 const std::function<void()> &f)
    {
        try
        {
            logf_trace("%s: Invoked receiver %s",
                       this->name_,
                       receiver);
            f();
            logf_trace("%s: Receiver %s returned",
                       this->name_,
                       receiver);
        }
        catch (...)
        {
            logf_notice("%s: Receiver %s failed: %s",
                        this->name_,
                        receiver,
                        std::current_exception());
        }
    }

}  // namespace cc::signal
