/// -*- c++ -*-
//==============================================================================
/// @file sync-dispatcher.h++
/// @brief Distribute a captured item to available sinks - blocking flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dispatcher.h++"
#include <mutex>

namespace cc::logging
{
    class SyncDispatcher : public Dispatcher
    {
        using Super = Dispatcher;

    public:
        void submit(const types::Loggable::Ref &item) override;

    private:
        std::mutex mtx_;
    };

}  // namespace cc::logging
