/// -*- c++ -*-
//==============================================================================
/// @file async-void-signal.h++
/// @brief Value-less signal type with asynchronous emit
/// @author Tor Slettnes
//==============================================================================

#include "signal/async-void-signal.h++"

namespace cc::core::signal
{
    std::size_t AsyncVoidSignal::emit()
    {
        this->set();

        Futures futures;
        this->signal_mtx.lock();
        futures.reserve(this->slots_.size());
        for (const auto &[receiver, method] : this->slots_)
        {
            futures.push_back(std::async(&AsyncVoidSignal::callback, this, receiver, method));
        }
        this->signal_mtx.unlock();
        return this->collect_futures(futures);
    }

}  // namespace cc::core::signal
