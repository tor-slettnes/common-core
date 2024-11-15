/// -*- c++ -*-
//==============================================================================
/// @file sync-dispatcher.c++
/// @brief Distribute a captured item to available sinks - blocking flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sync-dispatcher.h++"
#include "../sinks/core-sinks.h++"

namespace core::logging
{
    void SyncDispatcher::submit(const types::Loggable::ptr &item)
    {
        std::lock_guard<std::mutex> lck(this->mtx_);

        for (const auto &[sink_id, sink] : this->sinks())
        {
            if (sink->is_applicable(*item))
            {
                sink->capture(item);
            }
        }
    }

    SyncDispatcher message_dispatcher;
}  // namespace core::logging
