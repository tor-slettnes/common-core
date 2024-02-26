/// -*- c++ -*-
//==============================================================================
/// @file async-dispatcher.c++
/// @brief Distribute a captured item to available sinks - blocking flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sync-dispatcher.h++"

namespace core::logging
{
    void SyncDispatcher::submit(const types::Loggable::Ref &item)
    {
        std::lock_guard<std::mutex> lck(this->mtx_);

        for (const Sink::Ref &sink : this->sinks())
        {
            if (sink->is_applicable(*item))
            {
                sink->capture(item);
            }
        }
    }
}  // namespace core::logging
