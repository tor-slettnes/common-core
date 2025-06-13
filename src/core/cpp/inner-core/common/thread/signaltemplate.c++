/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.c++
/// @brief Adaptation of Signal/Slot pattern - inline functions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "signaltemplate.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "platform/symbols.h++"

namespace core::signal
{
    define_log_scope("signal");

    //==========================================================================
    /// @class BaseSignal
    /// @brief Abstract base for VoidSignal, DataSignal<T> and MappingSignal<T>

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

    std::string BaseSignal::name() const
    {
        return this->name_;
    }

    Handle BaseSignal::unique_handle() const
    {
        return platform::symbols->uuid();
    }

    bool BaseSignal::safe_invoke(const std::string &receiver,
                                 const std::function<void()> &f)
    {
        try
        {
            logf_trace("%s: Invoking receiver %s",
                       this->name_,
                       receiver);
            f();
            logf_trace("%s: Receiver %s completed",
                       this->name_,
                       receiver);
            return true;
        }
        catch (...)
        {
            logf_notice("%s: Receiver %s failed: %s",
                        this->name_,
                        receiver,
                        std::current_exception());
            return false;
        }
    }

    std::size_t BaseSignal::collect_futures(Futures &futures)
    {
        std::size_t count = 0;
        for (std::future<bool> &future : futures)
        {
            count += future.get();
        }
        return count;
    }

    //==========================================================================
    /// @class VoidSignal
    /// @brief Signal without data

    VoidSignal::VoidSignal(const std::string &id)
        : Super(id, false),
          emitted_(false)
    {
    }

    Handle VoidSignal::connect(const Slot &slot)
    {
        Handle handle(this->unique_handle());
        this->connect(handle, slot);
        return handle;
    }

    void VoidSignal::connect(const Handle &handle, const Slot &slot)
    {
        std::scoped_lock lck(this->mtx_);
        this->slots_[handle] = slot;
    }

    void VoidSignal::disconnect(const Handle &handle)
    {
        std::scoped_lock lck(this->mtx_);
        this->slots_.erase(handle);
    }

    std::size_t VoidSignal::emit()
    {
        std::scoped_lock lck(this->mtx_);
        std::size_t count = 0;
        for (const auto &[receiver, method] : this->slots_)
        {
            count += this->callback(receiver, method);
        }
        this->emitted_ = true;
        return count;
    }

    bool VoidSignal::emitted() const
    {
        return this->emitted_;
    }

    std::size_t VoidSignal::connection_count() const
    {
        return this->slots_.size();
    }

    bool VoidSignal::callback(const std::string &receiver, const Slot &method)
    {
        return this->safe_invoke(str::format("%s()", receiver), method);
    }


    //==========================================================================
    /// @class AsyncVoidSignal
    /// @brief Signal without data, emitted in parallel to all slots


    std::size_t AsyncVoidSignal::emit()
    {
        Futures futures;

        this->mtx_.lock();
        futures.reserve(this->slots_.size());
        for (const auto &[receiver, method] : this->slots_)
        {
            futures.push_back(std::async(&AsyncVoidSignal::callback, this, receiver, method));
        }
        this->emitted_ = true;
        this->mtx_.unlock();
        return this->collect_futures(futures);
    }


}  // namespace core::signal
