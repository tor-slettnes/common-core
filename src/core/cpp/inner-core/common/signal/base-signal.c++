/// -*- c++ -*-
//==============================================================================
/// @file base-signal.c++
/// @brief Adaptation of Signal/Slot pattern - inline functions
/// @author Tor Slettnes
//==============================================================================

#include "signal/base-signal.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"

namespace cc::core::signal
{
    define_log_scope("signal", status::Level::DEBUG);

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

    bool BaseSignal::caching() const
    {
        return this->caching_;
    }

    std::string BaseSignal::name() const
    {
        return this->name_;
    }

    void BaseSignal::to_stream(std::ostream &stream) const
    {
        stream << "Signal("
               << std::quoted(this->name())
               << ")";
    }

    Handle BaseSignal::unique_handle() const
    {
        return platform::symbols->uuid_string();
    }

    bool BaseSignal::safe_invoke(const std::string &receiver,
                                 const std::function<void()> &f)
    {
        try
        {
            logf_trace("%s: Invoking receiver: %s",
                       this->name_,
                       receiver);
            f();
            logf_trace("%s: Receiver completed: %s",
                       this->name_,
                       receiver);
            return true;
        }
        catch (...)
        {
            logf_notice("%s: Receiver failed: %s: %s",
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

}  // namespace cc::core::signal
