/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.c++
/// @brief Simple non-buffered adaptation of Signal/Slot pattern
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "signaltemplate.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace shared::signal
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

    //==========================================================================
    /// \class VoidSignal
    /// \brief Signal without data

    VoidSignal::VoidSignal(const std::string &id)
        : Super(id, false)
    {
    }

    const VoidSignal::Slot &VoidSignal::connect(const std::string &id, const Slot &slot)
    {
        std::scoped_lock lck(this->mtx_);
        this->slots_[id] = slot;
        return slot;
    }

    void VoidSignal::disconnect(const std::string &id)
    {
        std::scoped_lock lck(this->mtx_);
        this->slots_.erase(id);
    }

    size_t VoidSignal::emit()
    {
        std::scoped_lock lck(this->mtx_);
        for (const auto &cb : this->slots_)
        {
            this->callback(cb.first, cb.second);
        }
        return this->slots_.size();
    }

    size_t VoidSignal::connection_count()
    {
        return this->slots_.size();
    }

    void VoidSignal::callback(const std::string &receiver, const Slot &method)
    {
        this->safe_invoke(str::format("%s()", receiver), method);
    }

}  // namespace shared::signal
