/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.h++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/sinks/sink.h++"
#include "types/loggable.h++"

#include <memory>
#include <set>

namespace shared::logging
{
    class Dispatcher
    {
    public:
        virtual Sink::Ref add_sink(const Sink::Ref &sink);
        virtual bool remove_sink(const Sink::Ref &sink);
        const std::set<Sink::Ref> &sinks() const;

        virtual void initialize();
        virtual void deinitialize();

        virtual bool is_applicable(const types::Loggable &item) const;
        virtual void submit(const types::Loggable::Ref &item) = 0;

    protected:
        std::set<Sink::Ref> sinks_;
    };

}  // namespace shared::logging
