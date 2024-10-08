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

namespace core::logging
{
    class Dispatcher
    {
    public:
        virtual Sink::ptr add_sink(const Sink::ptr &sink);
        virtual bool remove_sink(const Sink::ptr &sink);
        const std::set<Sink::ptr> &sinks() const;

        virtual void initialize();
        virtual void deinitialize();

        virtual bool is_applicable(const types::Loggable &item) const;
        virtual void submit(const types::Loggable::ptr &item) = 0;

    protected:
        std::set<Sink::ptr> sinks_;
    };

}  // namespace core::logging
